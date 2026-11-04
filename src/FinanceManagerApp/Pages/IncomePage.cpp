#include "IncomePage.hpp"

#include "../Popups/ConfirmPopup.hpp"
#include "../Popups/CreatePopups.hpp"
#include "../Popups/EntryPopups.hpp"
#include "../Theme.hpp"
#include "../Ui.hpp"

#include <ranges>

namespace FinMgr {

    IncomePage::IncomePage(Store& store):
    m_Store(store) {

    }

    SP<App::Node> IncomePage::buildLayout() {
        using namespace App;
        const bool hasEntries = getSelectedWage() != nullptr && !m_Store.data.wageManager.getWage(*getSelectedWage()).getEntries().empty();

        return verticalSplit({.name = "IncomePage"},
            createPanel({.name = "Income Table", .drawFunction = bindDraw(this, &IncomePage::drawTable)}),
            m_Store.ui.showCharts && hasEntries ? createPanel({.name = "Income Chart", .drawFunction = bindDraw(this, &IncomePage::drawChart), .sizeRatio = 0.35f}) : nullptr
        );
    }

    const std::string* IncomePage::getSelectedWage() const {
        const auto& wages = m_Store.data.wageManager.getWages();
        if (const auto it = wages.find(m_Store.ui.selectedWage); it != wages.end()) {
            return &it->first;
        }
        const std::string* first = nullptr;
        for (const auto& name: wages | std::views::keys) {
            if (first == nullptr || name < *first) {
                first = &name;
            }
        }
        return first;
    }

    void IncomePage::drawTable() {
        const std::string* selected = getSelectedWage();
        if (selected == nullptr) {
            Ui::panelTitle("Income");
            if (Ui::emptyState("No income sources yet", "Add where your money comes from, like your salary or a side job, and log every paycheck.", "New income source")) {
                m_Store.popups.createIncome->open();
            }
            return;
        }
        const std::string name = *selected;
        Wage& wage = m_Store.data.wageManager.getWage(name);
        const WageTable table = buildWageTable(wage);

        const Date today = getCurrentDate();
        const WageStatistics thisYear = wage.calculateStatistics(Date(1, 1, today.year), Date(31, 12, today.year));
        const std::string subtitle = std::to_string(wage.getEntries().size()) + " paychecks, " + Ui::money(thisYear.totalAmount) + " in " + std::to_string(today.year);
        Ui::panelTitle(name.c_str(), subtitle.c_str());
        Ui::sameLineRight(Ui::buttonWidth("+ Add paycheck"));
        if (ImGui::Button("+ Add paycheck")) {
            m_Store.popups.addPaycheck->open(name);
        }

        if (table.empty()) {
            if (Ui::emptyState("No paychecks yet", "Log your paychecks to follow your income over the years.", "Add paycheck")) {
                m_Store.popups.addPaycheck->open(name);
            }
            return;
        }

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH;
        if (!ImGui::BeginTable("##Paychecks", 5, flags)) {
            return;
        }
        ImGui::TableSetupScrollFreeze(0, 2); // Header and all time totals
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("All time").x + ImGui::GetStyle().CellPadding.x * 2.0f);
        ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("30 September").x);
        ImGui::TableSetupColumn("Paycheck", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Extra", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        drawTotalRow("All time", table.totals, Theme::TotalRowBg);
        for (const auto& year: table.years) {
            drawTotalRow(std::to_string(year.year).c_str(), year.totals, Theme::YearRowBg);
            for (WageEntry* entry: year.entries) {
                drawEntryRow(name, *entry);
            }
        }
        ImGui::EndTable();
    }

    void IncomePage::drawTotalRow(const char* label, const WageTotals& totals, const ImVec4& background) {
        ImGui::TableNextRow();
        Ui::rowBackground(background);
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted("Total");
        ImGui::TableSetColumnIndex(2);
        Ui::amount(totals.check);
        ImGui::TableSetColumnIndex(3);
        Ui::amount(totals.extra);
        ImGui::TableSetColumnIndex(4);
        Ui::amount(totals.total());
    }

    void IncomePage::drawEntryRow(const std::string& wageName, WageEntry& entry) {
        ImGui::TableNextRow();
        ImGui::PushID(&entry);
        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        const std::string date = std::to_string(entry.date.day) + " " + Ui::monthName(entry.date.month);
        ImGui::Selectable(date.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);
        if (ImGui::BeginPopupContextItem("##Context")) {
            if (ImGui::MenuItem("Delete paycheck")) {
                const Date entryDate = entry.date;
                const std::string message = "Delete the " + wageName + " paycheck of " + entry.date.toString() + "?";
                m_Store.popups.confirm->ask(message, "Delete", [this, wageName, entryDate] {
                    if (!m_Store.data.wageManager.hasWage(wageName)) {
                        return;
                    }
                    Wage& wage = m_Store.data.wageManager.getWage(wageName);
                    const auto it = std::ranges::find_if(wage.getEntries(), [&](const WageEntry& e) { return e.date == entryDate; });
                    if (it != wage.getEntries().end()) {
                        wage.removeEntry(&*it);
                    }
                });
            }
            ImGui::EndPopup();
        }
        ImGui::TableSetColumnIndex(2);
        Ui::amountInput("##Check", &entry.amount);
        ImGui::TableSetColumnIndex(3);
        Ui::amountInput("##Extra", &entry.extra);
        ImGui::TableSetColumnIndex(4);
        Ui::amount(entry.amount + entry.extra);
        ImGui::PopID();
    }

    void IncomePage::drawChart() {
        const std::string* selected = getSelectedWage();
        if (selected == nullptr) {
            return;
        }
        const Wage& wage = m_Store.data.wageManager.getWage(*selected);
        if (wage.getEntries().empty()) {
            return;
        }
        const YearMonth today = YearMonth::of(getCurrentDate());
        const YearMonth first = YearMonth::of(wage.getEntries().front().date);
        const YearMonth last = std::max(today, YearMonth::of(wage.getEntries().back().date));

        const MonthlySeries series = buildWageSeries(*selected, wage, first, last);

        // Average of the last 12 months that are over, so a missing paycheck this month does not pull it down
        double sum = 0.0;
        int count = 0;
        for (Size i = 0; i < series.values.size(); ++i) {
            const YearMonth month = first.plusMonths(static_cast<int>(i));
            if (month < today && monthCount(month, today) <= 13) {
                sum += series.values[i];
                ++count;
            }
        }
        const double average = count > 0 ? sum / count : 0.0;

        Ui::panelTitle("Monthly income");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Average over the last %d months: %s", count, Ui::money(average).c_str());

        if (!Ui::beginTimePlot("##IncomeChart")) {
            return;
        }
        ImPlotSpec bars;
        bars.FillColor = Theme::Income;
        bars.FillAlpha = 0.75f;
        bars.LineColor = ImVec4(0, 0, 0, 0);
        ImPlot::PlotBars("Income", series.times.data(), series.values.data(), static_cast<int>(series.times.size()), SECONDS_PER_MONTH * 0.7, bars);

        ImPlotSpec averageSpec;
        averageSpec.LineColor = Theme::Trend;
        averageSpec.LineWeight = 2.0f;
        averageSpec.Flags = ImPlotInfLinesFlags_Horizontal;
        ImPlot::PlotInfLines("Average (12 months)", &average, 1, averageSpec);

        if (const int index = Ui::hoveredIndex(series.times); index >= 0) {
            const YearMonth month = first.plusMonths(index);
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(Ui::monthYear(month.month, month.year).c_str());
            ImGui::TextColored(Theme::Income, "%s", Ui::money(series.values[index]).c_str());
            ImGui::EndTooltip();
        }
        ImPlot::EndPlot();
    }

}
