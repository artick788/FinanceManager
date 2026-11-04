#include "ExpensesPage.hpp"

#include "../Popups/ConfirmPopup.hpp"
#include "../Popups/CreatePopups.hpp"
#include "../Popups/EntryPopups.hpp"
#include "../Theme.hpp"
#include "../Ui.hpp"

#include <ranges>

namespace FinMgr {

    namespace {

        constexpr int FIXED_COLUMNS = 2; // Year and month
        constexpr int FROZEN_ROWS = 2;   // Header and the all time totals

    }

    ExpensesPage::ExpensesPage(Store& store):
    m_Store(store) {

    }

    SP<App::Node> ExpensesPage::buildLayout() {
        using namespace App;
        m_Today = YearMonth::of(getCurrentDate());

        return verticalSplit({.name = "ExpensesPage"},
            createPanel({.name = "Expenses Table", .drawFunction = bindDraw(this, &ExpensesPage::drawTable)}),
            m_Store.ui.showCharts ? createPanel({.name = "Expenses Chart", .drawFunction = bindDraw(this, &ExpensesPage::drawChart), .sizeRatio = 0.35f}) : nullptr
        );
    }

    void ExpensesPage::drawTable() {
        auto& manager = m_Store.data.monthlyExpensesManager;
        const ExpenseTable table = buildExpenseTable(manager, getCurrentDate());

        const std::string subtitle = std::to_string(table.columns.size()) + " expenses, newest first";
        Ui::panelTitle("Monthly expenses", subtitle.c_str());
        Ui::sameLineRight(Ui::buttonWidth("+ Add entry") + Ui::buttonWidth("+ New expense") + ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("+ New expense")) {
            m_Store.popups.createMonthlyExpense->open();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(table.columns.empty());
        if (ImGui::Button("+ Add entry")) {
            m_Store.popups.addExpenseEntry->open("");
        }
        ImGui::EndDisabled();

        if (table.columns.empty()) {
            if (Ui::emptyState("No monthly expenses yet", "Add your recurring costs, like rent, insurance or subscriptions, to see where your money goes every month.", "New monthly expense")) {
                m_Store.popups.createMonthlyExpense->open();
            }
            return;
        }
        if (table.empty()) {
            if (Ui::emptyState("No entries yet", "Add a first entry, every following month is filled in automatically with the default amount.", "Add entry")) {
                m_Store.popups.addExpenseEntry->open("");
            }
            return;
        }

        const int columnCount = FIXED_COLUMNS + static_cast<int>(table.columns.size()) + 1;
        const ImGuiStyle& style = ImGui::GetStyle();
        // Wide enough for the largest amount of the table, which is always a total. Long names are truncated.
        const float amountWidth = ImGui::CalcTextSize(Ui::money(table.total).c_str()).x + style.FramePadding.x * 2.0f;
        const float yearWidth = ImGui::CalcTextSize("All time").x;
        const float monthWidth = ImGui::CalcTextSize("September").x;
        const float neededWidth = yearWidth + monthWidth + amountWidth * static_cast<float>(table.columns.size() + 1) +
            style.CellPadding.x * 2.0f * static_cast<float>(table.columns.size() + 3);

        // Amounts fill the panel when they fit, otherwise the table scrolls horizontally with year and month frozen
        const bool fits = ImGui::GetContentRegionAvail().x >= neededWidth;
        ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Hideable;
        if (!fits) {
            flags |= ImGuiTableFlags_ScrollX;
        }
        if (!ImGui::BeginTable("##MonthlyExpenses", columnCount, flags)) {
            return;
        }
        const ImGuiTableColumnFlags amountFlags = fits ? ImGuiTableColumnFlags_WidthStretch : ImGuiTableColumnFlags_WidthFixed;
        ImGui::TableSetupScrollFreeze(FIXED_COLUMNS, FROZEN_ROWS);
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, yearWidth);
        ImGui::TableSetupColumn("Month", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, monthWidth);
        for (const auto& name: table.columns) {
            ImGui::TableSetupColumn(name.c_str(), amountFlags, fits ? 0.0f : amountWidth);
        }
        ImGui::TableSetupColumn("Total", amountFlags | ImGuiTableColumnFlags_NoHide, fits ? 0.0f : amountWidth);
        ImGui::TableHeadersRow();

        // Exactly the old layout, drawn upside down: the totals of a year come before its months
        drawTotalRow("All time", "", table.totals, table.total, Theme::TotalRowBg);
        for (Size y = 0; y < table.years.size(); ++y) {
            const ExpenseYearGroup& year = table.years[y];
            const std::string yearLabel = std::to_string(year.year);
            drawTotalRow(yearLabel.c_str(), "Total", year.totals, year.total, Theme::YearRowBg);
            for (const auto& row: year.months) {
                drawMonthRow(table, row);
            }
            if (y + 1 < table.years.size()) {
                drawSpacerRow();
            }
        }
        ImGui::EndTable();
    }

    void ExpensesPage::drawTotalRow(const char* yearLabel, const char* monthLabel, const std::vector<double>& totals, const double total, const ImVec4& background) {
        ImGui::TableNextRow();
        Ui::rowBackground(background);
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(yearLabel);
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(monthLabel);
        for (Size col = 0; col < totals.size(); ++col) {
            ImGui::TableSetColumnIndex(FIXED_COLUMNS + static_cast<int>(col));
            if (totals[col] > 0.0) {
                Ui::amount(totals[col]);
            }
        }
        ImGui::TableSetColumnIndex(FIXED_COLUMNS + static_cast<int>(totals.size()));
        Ui::amount(total);
    }

    void ExpensesPage::drawMonthRow(const ExpenseTable& table, const ExpenseMonthRow& row) {
        ImGui::TableNextRow();
        ImGui::PushID(row.month.year * 100 + row.month.month);

        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        if (row.month == m_Today) {
            ImGui::TextColored(Theme::Accent, "%s", Ui::monthName(row.month.month));
        }
        else {
            ImGui::TextUnformatted(Ui::monthName(row.month.month));
        }

        for (Size col = 0; col < row.cells.size(); ++col) {
            ImGui::TableSetColumnIndex(FIXED_COLUMNS + static_cast<int>(col));
            ImGui::PushID(static_cast<int>(col));
            drawCell(table.columns[col], row.cells[col], row.month);
            ImGui::PopID();
        }

        ImGui::TableSetColumnIndex(FIXED_COLUMNS + static_cast<int>(row.cells.size()));
        if (row.total > 0.0) {
            Ui::amount(row.total);
        }
        ImGui::PopID();
    }

    void ExpensesPage::drawCell(const std::string& expenseName, const ExpenseCell& cell, const YearMonth month) {
        if (cell.entry != nullptr) {
            Ui::amountInput("##Amount", &cell.entry->amount);
            if (ImGui::BeginPopupContextItem("##Context")) {
                ImGui::TextDisabled("%s, %s", expenseName.c_str(), Ui::monthYear(month.month, month.year).c_str());
                ImGui::Separator();
                if (ImGui::MenuItem("Delete entry")) {
                    const Date date = cell.entry->date;
                    const std::string message = "Delete the " + expenseName + " entry of " + Ui::monthYear(month.month, month.year) + "?";
                    m_Store.popups.confirm->ask(message, "Delete", [this, expenseName, date] {
                        auto& manager = m_Store.data.monthlyExpensesManager;
                        if (!manager.hasMonthlyExpense(expenseName)) {
                            return;
                        }
                        MonthlyExpense& expense = manager.getMonthlyExpense(expenseName);
                        const auto it = std::ranges::find_if(expense.getEntries(), [&](const auto& e) { return e.date == date; });
                        if (it != expense.getEntries().end()) {
                            expense.removeEntry(&*it);
                        }
                    });
                }
                ImGui::EndPopup();
            }
            return;
        }
        if (!cell.started) {
            return;
        }
        // A month without an entry, double click to fill it in
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Muted);
        ImGui::Selectable("N/A", false, ImGuiSelectableFlags_AllowDoubleClick);
        ImGui::PopStyleColor();
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            m_Store.popups.addExpenseEntry->open(expenseName, month);
        }
        ImGui::SetItemTooltip("No entry this month, double click to add one");
    }

    void ExpensesPage::drawSpacerRow() {
        ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetTextLineHeight() * 0.6f);
        ImGui::TableSetColumnIndex(0);
    }

    void ExpensesPage::drawChart() {
        const auto& manager = m_Store.data.monthlyExpensesManager;
        std::optional<YearMonth> first;
        for (const auto& expense: manager.getMonthlyExpenses() | std::views::values) {
            if (!expense.getEntries().empty()) {
                const YearMonth month = YearMonth::of(expense.getEntries().front().date);
                first = first ? std::min(*first, month) : month;
            }
        }
        if (!first) {
            Ui::emptyState("No data to plot yet", "The chart shows up once there are expense entries.");
            return;
        }

        const std::vector<MonthlySeries> series = buildExpenseSeries(manager, *first, m_Today);
        std::vector<double> times = series.front().times;
        std::vector<double> totals(times.size(), 0.0);
        for (const auto& s: series) {
            for (Size i = 0; i < s.values.size(); ++i) {
                totals[i] += s.values[i];
            }
        }

        const auto maxIt = std::ranges::max_element(totals);
        const Size maxIndex = std::distance(totals.begin(), maxIt);
        const auto minIt = std::ranges::min_element(totals);
        const Size minIndex = std::distance(totals.begin(), minIt);
        const YearMonth maxMonth = first->plusMonths(static_cast<int>(maxIndex));
        const YearMonth minMonth = first->plusMonths(static_cast<int>(minIndex));

        Ui::panelTitle("Expense trend");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Most expensive: %s (%s)   Cheapest: %s (%s)",
            Ui::monthYear(maxMonth.month, maxMonth.year).c_str(), Ui::money(*maxIt).c_str(),
            Ui::monthYear(minMonth.month, minMonth.year).c_str(), Ui::money(*minIt).c_str());

        if (!Ui::beginTimePlot("##ExpenseTrend")) {
            return;
        }
        // One line per expense, the legend would cover them inside the plot
        ImPlot::SetupLegend(ImPlotLocation_East, ImPlotLegendFlags_Outside);
        for (const auto& s: series) {
            ImPlotSpec spec;
            spec.LineWeight = 1.5f;
            ImPlot::PlotLine(s.name.c_str(), s.times.data(), s.values.data(), static_cast<int>(s.times.size()), spec);
        }
        ImPlotSpec totalSpec;
        totalSpec.LineColor = Theme::Expenses;
        totalSpec.LineWeight = 3.0f;
        ImPlot::PlotLine("Total", times.data(), totals.data(), static_cast<int>(times.size()), totalSpec);

        if (const int index = Ui::hoveredIndex(times); index >= 0) {
            Ui::hoverLine(times[index]);
            const YearMonth month = first->plusMonths(index);
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(Ui::monthYear(month.month, month.year).c_str());
            ImGui::Separator();
            for (const auto& s: series) {
                if (s.values[index] != 0.0) {
                    ImGui::Text("%-20s %s", s.name.c_str(), Ui::money(s.values[index]).c_str());
                }
            }
            ImGui::TextColored(Theme::Expenses, "%-20s %s", "Total", Ui::money(totals[index]).c_str());
            ImGui::EndTooltip();
        }
        ImPlot::EndPlot();
    }

}
