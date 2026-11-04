#include "DashboardPage.hpp"

#include "../Popups/CreatePopups.hpp"
#include "../Theme.hpp"
#include "../Ui.hpp"

#include <cmath>
#include <numeric>

namespace FinMgr {

    namespace {

        struct Averages {
            double income = 0.0;
            double expenses = 0.0;
            int months = 0;

            double net() const { return income - expenses; }

            double savingsRate() const { return income > 0.0 ? net() / income : 0.0; }
        };

        /**
         * @brief Averages over the months before the current one, the current month is usually not complete yet
         */
        Averages averageOfCompletedMonths(const CashFlow& flow, const YearMonth today, const int maxMonths) {
            Averages averages;
            for (Size i = flow.months.size(); i-- > 0 && averages.months < maxMonths;) {
                if (flow.months[i] >= today) {
                    continue;
                }
                averages.income += flow.income[i];
                averages.expenses += flow.expenses[i];
                ++averages.months;
            }
            if (averages.months > 0) {
                averages.income /= averages.months;
                averages.expenses /= averages.months;
            }
            return averages;
        }

        std::string comparedTo(const double value, const double reference, const char* what) {
            if (reference == 0.0) {
                return "";
            }
            const double change = (value - reference) / std::fabs(reference);
            return (change >= 0.0 ? "+" : "") + Ui::percentage(change) + " vs " + what;
        }

    }

    DashboardPage::DashboardPage(Store& store):
    m_Store(store) {

    }

    SP<App::Node> DashboardPage::buildLayout() {
        using namespace App;
        m_Today = YearMonth::of(getCurrentDate());
        m_FirstMonth = firstMonthWithData(m_Store.data);
        // All months, the panels pick the range they need
        m_CashFlow = m_FirstMonth ? buildCashFlow(m_Store.data, std::min(*m_FirstMonth, m_Today), m_Today) : CashFlow{};

        if (!m_FirstMonth) {
            return createPanel({.name = "Dashboard Welcome", .drawFunction = bindDraw(this, &DashboardPage::drawOverview)});
        }
        return verticalSplit({.name = "Dashboard"},
            createPanel({.name = "Dashboard Overview", .drawFunction = bindDraw(this, &DashboardPage::drawOverview), .sizeRatio = 0.37f}),
            horizontalSplit({.name = "DashboardCharts"},
                createPanel({.name = "Dashboard Cash Flow", .drawFunction = bindDraw(this, &DashboardPage::drawCashFlow), .sizeRatio = 0.6f}),
                verticalSplit({.name = "DashboardRight"},
                    createPanel({.name = "Dashboard Savings", .drawFunction = bindDraw(this, &DashboardPage::drawSavings), .sizeRatio = 0.5f}),
                    createPanel({.name = "Dashboard Fixed Costs", .drawFunction = bindDraw(this, &DashboardPage::drawFixedCosts)})
                )
            )
        );
    }

    YearMonth DashboardPage::getChartStart() const {
        const YearMonth first = m_FirstMonth.value_or(m_Today);
        if (m_Store.ui.chartMonths <= 0) {
            return std::min(first, m_Today);
        }
        return std::max(first, m_Today.plusMonths(1 - m_Store.ui.chartMonths));
    }

    bool DashboardPage::drawRangeSelector() {
        const int options[] = {6, 12, 24, 36, 0};
        const char* labels[] = {"6M", "1Y", "2Y", "3Y", "All"};
        float width = 0.0f;
        for (const char* label: labels) {
            width += Ui::buttonWidth(label) + ImGui::GetStyle().ItemSpacing.x;
        }
        Ui::sameLineRight(width - ImGui::GetStyle().ItemSpacing.x);
        bool changed = false;
        for (int i = 0; i < 5; ++i) {
            if (i > 0) {
                ImGui::SameLine();
            }
            const bool active = m_Store.ui.chartMonths == options[i];
            if (active) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.6f));
            }
            if (ImGui::Button(labels[i])) {
                m_Store.ui.chartMonths = options[i];
                changed = true;
            }
            if (active) {
                ImGui::PopStyleColor();
            }
        }
        return changed;
    }

    // -----------------------------------------------------------------------------------------------------------------

    void DashboardPage::drawOverview() {
        const std::string title = Ui::monthYear(m_Today.month, m_Today.year);
        Ui::panelTitle("Overview", title.c_str());

        if (!m_FirstMonth) {
            ImGui::Spacing();
            Ui::emptyState("Welcome to FinanceManager",
                "Start by adding an income source, a savings account or a monthly expense. "
                "The dashboard fills up as soon as you log your first entries.");
            const float width = Ui::buttonWidth("+ Income source") + Ui::buttonWidth("+ Savings account") + Ui::buttonWidth("+ Monthly expense") + ImGui::GetStyle().ItemSpacing.x * 2.0f;
            ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetContentRegionAvail().x - width) * 0.5f));
            if (ImGui::Button("+ Income source")) {
                m_Store.popups.createIncome->open();
            }
            ImGui::SameLine();
            if (ImGui::Button("+ Savings account")) {
                m_Store.popups.createSavingsRecord->open();
            }
            ImGui::SameLine();
            if (ImGui::Button("+ Monthly expense")) {
                m_Store.popups.createMonthlyExpense->open();
            }
            return;
        }

        const Size last = m_CashFlow.months.size() - 1;
        const double incomeThisMonth = m_CashFlow.income[last];
        const double expensesThisMonth = m_CashFlow.expenses[last];
        const double netThisMonth = m_CashFlow.net[last];
        const Averages averages = averageOfCompletedMonths(m_CashFlow, m_Today, 12);

        double incomeThisYear = 0.0;
        double expensesThisYear = 0.0;
        for (Size i = 0; i < m_CashFlow.months.size(); ++i) {
            if (m_CashFlow.months[i].year == m_Today.year) {
                incomeThisYear += m_CashFlow.income[i];
                expensesThisYear += m_CashFlow.expenses[i];
            }
        }
        const SavingsSnapshot savings = currentSavings(m_Store.data.savingsManager);

        const std::string averageLabel = "the " + std::to_string(averages.months) + " month average";
        const Ui::Kpi thisMonth[] = {
            {.label = "Income this month", .value = Ui::money(incomeThisMonth),
             .detail = incomeThisMonth > 0.0 ? comparedTo(incomeThisMonth, averages.income, averageLabel.c_str()) : "No paycheck logged yet",
             .valueColor = Theme::Income},
            {.label = "Expenses this month", .value = Ui::money(expensesThisMonth),
             .detail = comparedTo(expensesThisMonth, averages.expenses, averageLabel.c_str()), .valueColor = Theme::Expenses},
            {.label = "Left over this month", .value = Ui::signedMoney(netThisMonth),
             .detail = incomeThisMonth > 0.0 ? Ui::percentage(netThisMonth / incomeThisMonth) + " of income" : "Income minus expenses",
             .valueColor = netThisMonth >= 0.0 ? Theme::Positive : Theme::Negative},
            {.label = "Savings balance", .value = Ui::money(savings.actual),
             .detail = Ui::signedMoney(savings.variance()) + " vs expected",
             .valueColor = Theme::Savings, .detailColor = savings.variance() >= 0.0 ? Theme::Positive : Theme::Negative},
        };
        Ui::kpiRow("ThisMonth", thisMonth, 4);

        const std::string yearLabel = std::to_string(m_Today.year) + " so far";
        const Ui::Kpi longTerm[] = {
            {.label = "Average income", .value = Ui::money(averages.income), .detail = "per month, last " + std::to_string(averages.months) + " months"},
            {.label = "Average expenses", .value = Ui::money(averages.expenses), .detail = "per month, last " + std::to_string(averages.months) + " months"},
            {.label = "Savings rate", .value = Ui::percentage(averages.savingsRate()), .detail = "of income left over on average",
             .valueColor = averages.savingsRate() >= 0.0 ? Theme::Positive : Theme::Negative},
            {.label = yearLabel.c_str(), .value = Ui::signedMoney(incomeThisYear - expensesThisYear),
             .detail = Ui::money(incomeThisYear) + " in, " + Ui::money(expensesThisYear) + " out"},
        };
        Ui::kpiRow("LongTerm", longTerm, 4);
    }

    void DashboardPage::drawCashFlow() {
        Ui::panelTitle("Cash flow");
        drawRangeSelector();

        const YearMonth start = getChartStart();
        const int offset = monthCount(m_CashFlow.months.front(), start) - 1;
        const int count = static_cast<int>(m_CashFlow.months.size()) - offset;
        if (count <= 0) {
            return;
        }
        const double* times = m_CashFlow.times.data() + offset;

        if (!Ui::beginTimePlot("##CashFlow")) {
            return;
        }
        // Income and expenses side by side within the month
        const double barWidth = SECONDS_PER_MONTH * 0.36;
        std::vector<double> incomeX(times, times + count);
        std::vector<double> expenseX(times, times + count);
        for (int i = 0; i < count; ++i) {
            incomeX[i] -= barWidth * 0.5;
            expenseX[i] += barWidth * 0.5;
        }
        ImPlotSpec income;
        income.FillColor = Theme::Income;
        income.FillAlpha = 0.8f;
        income.LineColor = ImVec4(0, 0, 0, 0);
        ImPlot::PlotBars("Income", incomeX.data(), m_CashFlow.income.data() + offset, count, barWidth, income);

        ImPlotSpec expenses;
        expenses.FillColor = Theme::Expenses;
        expenses.FillAlpha = 0.8f;
        expenses.LineColor = ImVec4(0, 0, 0, 0);
        ImPlot::PlotBars("Expenses", expenseX.data(), m_CashFlow.expenses.data() + offset, count, barWidth, expenses);

        ImPlotSpec net;
        net.LineColor = Theme::Trend;
        net.LineWeight = 2.0f;
        net.Marker = ImPlotMarker_Circle;
        net.MarkerSize = 3.0f;
        ImPlot::PlotLine("Left over", times, m_CashFlow.net.data() + offset, count, net);

        const std::vector<double> visibleTimes(times, times + count);
        if (const int index = Ui::hoveredIndex(visibleTimes); index >= 0) {
            const Size i = static_cast<Size>(offset + index);
            const YearMonth month = m_CashFlow.months[i];
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(Ui::monthYear(month.month, month.year).c_str());
            ImGui::Separator();
            ImGui::TextColored(Theme::Income, "Income     %s", Ui::money(m_CashFlow.income[i]).c_str());
            ImGui::TextColored(Theme::Expenses, "Expenses   %s", Ui::money(m_CashFlow.expenses[i]).c_str());
            ImGui::TextColored(m_CashFlow.net[i] >= 0.0 ? Theme::Positive : Theme::Negative, "Left over  %s", Ui::signedMoney(m_CashFlow.net[i]).c_str());
            ImGui::EndTooltip();
        }
        ImPlot::EndPlot();
    }

    void DashboardPage::drawSavings() {
        const SavingsTrend full = buildTotalSavingsTrend(m_Store.data.savingsManager, m_Today);
        if (full.empty()) {
            Ui::panelTitle("Savings");
            if (Ui::emptyState("No savings yet", "Log the balance of your savings accounts to see them grow.", "New savings account")) {
                m_Store.popups.createSavingsRecord->open();
            }
            return;
        }
        // Same range as the cash flow chart
        const YearMonth start = std::max(getChartStart(), full.months.front());
        const int offset = monthCount(full.months.front(), start) - 1;
        const int count = static_cast<int>(full.months.size()) - offset;
        const double change = full.actual.back() - full.actual[offset];

        Ui::panelTitle("Savings");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(change >= 0.0 ? Theme::Positive : Theme::Negative, "%s", Ui::signedMoney(change).c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("since %s", Ui::monthYear(start.month, start.year).c_str());

        if (!Ui::beginTimePlot("##DashboardSavings", ImVec2(-1.0f, -1.0f), true)) {
            return;
        }
        ImPlotSpec fill;
        fill.FillColor = Theme::Savings;
        fill.FillAlpha = 0.15f;
        fill.Flags = ImPlotItemFlags_NoLegend;
        ImPlot::PlotShaded("##Fill", full.times.data() + offset, full.actual.data() + offset, count, -INFINITY, fill);
        ImPlotSpec actual;
        actual.LineColor = Theme::Savings;
        actual.LineWeight = 2.5f;
        ImPlot::PlotLine("Balance", full.times.data() + offset, full.actual.data() + offset, count, actual);
        ImPlotSpec expected;
        expected.LineColor = Theme::Expected;
        expected.LineWeight = 1.5f;
        ImPlot::PlotLine("Expected", full.times.data() + offset, full.expected.data() + offset, count, expected);

        const std::vector<double> visibleTimes(full.times.begin() + offset, full.times.end());
        if (const int index = Ui::hoveredIndex(visibleTimes); index >= 0) {
            const Size i = static_cast<Size>(offset + index);
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(Ui::monthYear(full.months[i].month, full.months[i].year).c_str());
            ImGui::TextColored(Theme::Savings, "Balance   %s", Ui::money(full.actual[i]).c_str());
            ImGui::TextColored(Theme::Expected, "Expected  %s", Ui::money(full.expected[i]).c_str());
            ImGui::EndTooltip();
        }
        ImPlot::EndPlot();
    }

    void DashboardPage::drawFixedCosts() {
        const std::vector<RecurringExpense> expenses = currentRecurringExpenses(m_Store.data.monthlyExpensesManager, getCurrentDate());
        const double total = std::accumulate(expenses.begin(), expenses.end(), 0.0,
            [](const double sum, const RecurringExpense& e) { return sum + e.amount; });
        const Averages averages = averageOfCompletedMonths(m_CashFlow, m_Today, 12);

        Ui::panelTitle("Fixed costs");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("%s per month", Ui::money(total).c_str());
        if (averages.income > 0.0) {
            ImGui::SameLine();
            ImGui::TextDisabled("(%s of income)", Ui::percentage(total / averages.income).c_str());
        }

        if (expenses.empty()) {
            if (Ui::emptyState("No monthly expenses yet", "Recurring costs show up here, from most to least expensive.", "New monthly expense")) {
                m_Store.popups.createMonthlyExpense->open();
            }
            return;
        }

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg;
        if (!ImGui::BeginTable("##FixedCosts", 3, flags)) {
            return;
        }
        ImGui::TableSetupColumn("Expense", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Share", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Amount", ImGuiTableColumnFlags_WidthFixed);
        for (const auto& expense: expenses) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(expense.name.c_str());
            ImGui::SetItemTooltip("Paid around day %d of the month", expense.day);
            ImGui::TableSetColumnIndex(1);
            const float share = total > 0.0 ? static_cast<float>(expense.amount / total) : 0.0f;
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(Theme::Expenses.x, Theme::Expenses.y, Theme::Expenses.z, 0.75f));
            ImGui::ProgressBar(share, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight()), Ui::percentage(share).c_str());
            ImGui::PopStyleColor();
            ImGui::TableSetColumnIndex(2);
            Ui::amount(expense.amount);
        }
        ImGui::EndTable();
    }

}
