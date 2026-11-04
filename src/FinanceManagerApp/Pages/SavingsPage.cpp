#include "SavingsPage.hpp"

#include "../Popups/ConfirmPopup.hpp"
#include "../Popups/CreatePopups.hpp"
#include "../Popups/EntryPopups.hpp"
#include "../Theme.hpp"
#include "../Ui.hpp"

#include <cmath>
#include <ranges>

namespace FinMgr {

    namespace {

        void drawVariance(const double actual, const double expected) {
            const double variance = actual - expected;
            std::string text = Ui::signedMoney(variance);
            if (expected != 0.0) {
                text += " (" + Ui::percentage(variance / expected) + ")";
            }
            const double cents = std::round(variance * 100.0);
            const float inset = ImGui::GetStyle().FramePadding.x;
            if (cents == 0.0) {
                Ui::textRightAligned(text.c_str(), nullptr, inset);
            }
            else {
                const ImVec4& color = cents > 0.0 ? Theme::Positive : Theme::Negative;
                Ui::textRightAligned(text.c_str(), &color, inset);
            }
        }

    }

    SavingsPage::SavingsPage(Store& store):
    m_Store(store) {

    }

    SP<App::Node> SavingsPage::buildLayout() {
        using namespace App;
        const bool hasData = std::ranges::any_of(m_Store.data.savingsManager.getSavingsRecords() | std::views::values,
            [](const SavingsRecord& record) { return !record.getEntries().empty(); });

        SP<Node> main = getSelectedRecord() == nullptr
            ? createPanel({.name = "Savings Overview", .drawFunction = bindDraw(this, &SavingsPage::drawOverview)})
            : createPanel({.name = "Savings Table", .drawFunction = bindDraw(this, &SavingsPage::drawRecordTable)});

        return verticalSplit({.name = "SavingsPage"},
            main,
            m_Store.ui.showCharts && hasData ? createPanel({.name = "Savings Chart", .drawFunction = bindDraw(this, &SavingsPage::drawChart), .sizeRatio = 0.4f}) : nullptr
        );
    }

    const std::string* SavingsPage::getSelectedRecord() const {
        const auto& records = m_Store.data.savingsManager.getSavingsRecords();
        const auto it = records.find(m_Store.ui.selectedSavings);
        return it != records.end() ? &it->first : nullptr;
    }

    // -----------------------------------------------------------------------------------------------------------------

    void SavingsPage::drawOverview() {
        auto& manager = m_Store.data.savingsManager;
        Ui::panelTitle("All savings accounts");
        Ui::sameLineRight(Ui::buttonWidth("+ Add entry") + Ui::buttonWidth("+ New account") + ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("+ New account")) {
            m_Store.popups.createSavingsRecord->open();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled(manager.getSavingsRecords().empty());
        if (ImGui::Button("+ Add entry")) {
            m_Store.popups.addSavingsEntry->open("");
        }
        ImGui::EndDisabled();

        if (manager.getSavingsRecords().empty()) {
            if (Ui::emptyState("No savings accounts yet", "Add your savings accounts and log their balance every month to see how your savings grow compared to what you planned.", "New savings account")) {
                m_Store.popups.createSavingsRecord->open();
            }
            return;
        }

        const SavingsSnapshot snapshot = currentSavings(manager);
        const Ui::Kpi kpis[] = {
            {.label = "Total balance", .value = Ui::money(snapshot.actual)},
            {.label = "Expected", .value = Ui::money(snapshot.expected), .detail = "Initial amounts + planned deposits"},
            {.label = "Variance", .value = Ui::signedMoney(snapshot.variance()),
             .detail = snapshot.expected != 0.0 ? Ui::percentage(snapshot.variance() / snapshot.expected) + " of expected" : "",
             .valueColor = snapshot.variance() >= 0.0 ? Theme::Positive : Theme::Negative},
            {.label = "Deposited", .value = Ui::money(snapshot.deposited), .detail = "Sum of all planned deposits"},
        };
        Ui::kpiRow("SavingsKpis", kpis, 4);
        ImGui::Spacing();

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH |
            ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable;
        if (!ImGui::BeginTable("##Accounts", 6, flags)) {
            return;
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Account", ImGuiTableColumnFlags_WidthStretch | ImGuiTableColumnFlags_DefaultSort);
        ImGui::TableSetupColumn("Last entry", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Balance", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Expected", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Variance", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Share", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        struct Row {
            std::string name;
            Date lastDate;
            double actual = 0.0;
            double expected = 0.0;
        };
        std::vector<Row> rows;
        for (auto& [name, record]: manager.getSavingsRecords()) {
            const SavingsTable table = buildSavingsTable(record);
            rows.push_back({name, record.getEntries().empty() ? Date{} : record.getEntries().back().date, table.actual, table.expected});
        }
        if (ImGuiTableSortSpecs* sort = ImGui::TableGetSortSpecs(); sort != nullptr && sort->SpecsCount > 0) {
            const ImGuiTableColumnSortSpecs& spec = sort->Specs[0];
            const auto less = [&spec](const Row& a, const Row& b) {
                switch (spec.ColumnIndex) {
                    case 1: return a.lastDate < b.lastDate;
                    case 2: case 5: return a.actual < b.actual;
                    case 3: return a.expected < b.expected;
                    case 4: return a.actual - a.expected < b.actual - b.expected;
                    default: return a.name < b.name;
                }
            };
            const bool ascending = spec.SortDirection == ImGuiSortDirection_Ascending;
            std::ranges::stable_sort(rows, [&](const Row& a, const Row& b) { return ascending ? less(a, b) : less(b, a); });
        }

        for (const Row& row: rows) {
            ImGui::TableNextRow();
            ImGui::PushID(row.name.c_str());
            ImGui::TableSetColumnIndex(0);
            if (ImGui::Selectable(row.name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap)) {
                m_Store.ui.selectedSavings = row.name;
            }
            ImGui::SetItemTooltip("Open %s", row.name.c_str());
            ImGui::TableSetColumnIndex(1);
            if (row.lastDate.year > 0) {
                ImGui::TextUnformatted(row.lastDate.toString().c_str());
            }
            else {
                ImGui::TextDisabled("No entries");
            }
            ImGui::TableSetColumnIndex(2);
            Ui::amount(row.actual);
            ImGui::TableSetColumnIndex(3);
            Ui::amount(row.expected);
            ImGui::TableSetColumnIndex(4);
            drawVariance(row.actual, row.expected);
            ImGui::TableSetColumnIndex(5);
            const float share = snapshot.actual > 0.0 ? static_cast<float>(std::max(row.actual, 0.0) / snapshot.actual) : 0.0f;
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, Theme::Savings);
            ImGui::ProgressBar(share, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight()), Ui::percentage(share).c_str());
            ImGui::PopStyleColor();
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    // -----------------------------------------------------------------------------------------------------------------

    void SavingsPage::drawRecordTable() {
        // The layout of this frame was built before the navigator drew, which may have changed the selection since
        const std::string* selected = getSelectedRecord();
        if (selected == nullptr) {
            drawOverview();
            return;
        }
        const std::string name = *selected;
        SavingsRecord& record = m_Store.data.savingsManager.getSavingsRecord(name);
        const SavingsTable table = buildSavingsTable(record);

        const std::string subtitle = std::to_string(record.getEntries().size()) + " entries, newest first";
        Ui::panelTitle(name.c_str(), subtitle.c_str());
        Ui::sameLineRight(Ui::buttonWidth("+ Add entry") + Ui::buttonWidth("All accounts") + ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::Button("All accounts")) {
            m_Store.ui.selectedSavings.clear();
        }
        ImGui::SameLine();
        if (ImGui::Button("+ Add entry")) {
            m_Store.popups.addSavingsEntry->open(name);
        }

        const Ui::Kpi kpis[] = {
            {.label = "Balance", .value = Ui::money(table.actual)},
            {.label = "Expected", .value = Ui::money(table.expected)},
            {.label = "Variance", .value = Ui::signedMoney(table.variance()),
             .detail = table.expected != 0.0 ? Ui::percentage(table.variance() / table.expected) + " of expected" : "",
             .valueColor = table.variance() >= 0.0 ? Theme::Positive : Theme::Negative},
            {.label = "Gain", .value = Ui::signedMoney(table.actual - table.initialAmount - table.deposited),
             .detail = "Growth not from deposits"},
        };
        Ui::kpiRow("RecordKpis", kpis, 4);
        ImGui::Spacing();

        if (table.empty()) {
            if (Ui::emptyState("No entries yet", "Log the balance of this account every month to compare it with what you planned.", "Add entry")) {
                m_Store.popups.addSavingsEntry->open(name);
            }
            return;
        }

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY |
            ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH;
        if (!ImGui::BeginTable("##SavingsEntries", 7, flags)) {
            return;
        }
        ImGui::TableSetupScrollFreeze(0, 2);
        ImGui::TableSetupColumn("Year", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("Start").x + ImGui::GetStyle().CellPadding.x * 4.0f);
        ImGui::TableSetupColumn("Date", ImGuiTableColumnFlags_WidthFixed, ImGui::CalcTextSize("30 September").x);
        ImGui::TableSetupColumn("Deposit", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Expected", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Balance", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Change", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Variance", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        drawTotalRow("Now", "", table.deposited, table.expected, table.actual, table.actual - table.initialAmount, Theme::TotalRowBg, "Change since the initial amount");
        for (const auto& year: table.years) {
            const std::string tooltip = "Grew " + Ui::signedMoney(year.growth()) + " in " + std::to_string(year.year) + ": " +
                Ui::money(year.deposited) + " deposited and " + Ui::signedMoney(year.gain()) + " gain";
            drawTotalRow(std::to_string(year.year).c_str(), "Year end", year.deposited, year.endExpected, year.endActual, year.growth(), Theme::YearRowBg, tooltip);
            for (const SavingsRow& row: year.rows) {
                drawEntryRow(name, row);
            }
        }
        // The oldest row is the starting point of the record, the old app showed it on top
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::TextDisabled("Start");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextDisabled("Initial amount");
        ImGui::TableSetColumnIndex(3);
        Ui::amount(table.initialAmount);
        ImGui::TableSetColumnIndex(4);
        Ui::amountInput("##Initial", &record.initialAmount);
        ImGui::EndTable();
    }

    void SavingsPage::drawTotalRow(const char* label, const char* dateLabel, const double deposited, const double expected,
                                   const double actual, const double change, const ImVec4& background, const std::string& changeTooltip) {
        ImGui::TableNextRow();
        Ui::rowBackground(background);
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(dateLabel);
        ImGui::TableSetColumnIndex(2);
        Ui::amount(deposited);
        ImGui::TableSetColumnIndex(3);
        Ui::amount(expected);
        ImGui::TableSetColumnIndex(4);
        Ui::amount(actual);
        ImGui::TableSetColumnIndex(5);
        Ui::amount(change, true);
        ImGui::SetItemTooltip("%s", changeTooltip.c_str());
        ImGui::TableSetColumnIndex(6);
        drawVariance(actual, expected);
    }

    void SavingsPage::drawEntryRow(const std::string& recordName, const SavingsRow& row) {
        SavingEntry& entry = *row.entry;
        ImGui::TableNextRow();
        ImGui::PushID(&entry);
        ImGui::TableSetColumnIndex(1);
        ImGui::AlignTextToFramePadding();
        const std::string date = std::to_string(entry.date.day) + " " + Ui::monthName(entry.date.month);
        ImGui::Selectable(date.c_str(), false, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowOverlap);
        if (ImGui::BeginPopupContextItem("##Context")) {
            if (ImGui::MenuItem("Delete entry")) {
                const Date entryDate = entry.date;
                const std::string message = "Delete the " + recordName + " entry of " + entry.date.toString() + "?";
                m_Store.popups.confirm->ask(message, "Delete", [this, recordName, entryDate] {
                    if (!m_Store.data.savingsManager.hasSavingsRecord(recordName)) {
                        return;
                    }
                    SavingsRecord& record = m_Store.data.savingsManager.getSavingsRecord(recordName);
                    const auto it = std::ranges::find_if(record.getEntries(), [&](const SavingEntry& e) { return e.date == entryDate; });
                    if (it != record.getEntries().end()) {
                        record.removeEntry(&*it);
                    }
                });
            }
            ImGui::EndPopup();
        }
        ImGui::TableSetColumnIndex(2);
        Ui::amountInput("##Deposit", &entry.expectedTransactionAmount);
        ImGui::TableSetColumnIndex(3);
        Ui::amount(row.expected);
        ImGui::TableSetColumnIndex(4);
        Ui::amountInput("##Balance", &entry.actualState);
        ImGui::TableSetColumnIndex(5);
        Ui::amount(row.change, true);
        ImGui::TableSetColumnIndex(6);
        drawVariance(entry.actualState, row.expected);
        ImGui::PopID();
    }

    // -----------------------------------------------------------------------------------------------------------------

    void SavingsPage::drawChart() {
        const YearMonth today = YearMonth::of(getCurrentDate());
        if (const std::string* selected = getSelectedRecord()) {
            const SavingsTrend trend = buildSavingsTrend(m_Store.data.savingsManager.getSavingsRecord(*selected), today);
            drawTrendPlot("##RecordTrend", trend);
        }
        else {
            drawTrendPlot("##TotalTrend", buildTotalSavingsTrend(m_Store.data.savingsManager, today));
        }
    }

    void SavingsPage::drawTrendPlot(const char* id, const SavingsTrend& trend) {
        if (trend.empty()) {
            Ui::emptyState("No data to plot yet", "The chart shows up once this account has entries.");
            return;
        }
        const OLSLinearRegression fit = performOLSLinearRegression(trend.times, trend.actual);
        const double monthlyChange = fit.slope * SECONDS_PER_MONTH;

        Ui::panelTitle("Savings growth");
        ImGui::SameLine();
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Trend: %s per month", Ui::signedMoney(monthlyChange).c_str());

        if (!Ui::beginTimePlot(id)) {
            return;
        }
        const int count = static_cast<int>(trend.times.size());
        ImPlotSpec shaded;
        shaded.FillColor = Theme::Savings;
        shaded.FillAlpha = 0.15f;
        shaded.Flags = ImPlotItemFlags_NoLegend;
        ImPlot::PlotShaded("##ActualFill", trend.times.data(), trend.actual.data(), count, -INFINITY, shaded);

        ImPlotSpec actual;
        actual.LineColor = Theme::Savings;
        actual.LineWeight = 2.5f;
        ImPlot::PlotLine("Balance", trend.times.data(), trend.actual.data(), count, actual);

        ImPlotSpec expected;
        expected.LineColor = Theme::Expected;
        expected.LineWeight = 1.5f;
        ImPlot::PlotLine("Expected", trend.times.data(), trend.expected.data(), count, expected);

        const double trendX[2] = {trend.times.front(), trend.times.back()};
        const double trendY[2] = {fit.at(trendX[0]), fit.at(trendX[1])};
        ImPlotSpec trendSpec;
        trendSpec.LineColor = Theme::Trend;
        trendSpec.LineWeight = 1.0f;
        ImPlot::PlotLine("Trend", trendX, trendY, 2, trendSpec);

        if (const int index = Ui::hoveredIndex(trend.times); index >= 0) {
            Ui::hoverLine(trend.times[index]);
            const YearMonth month = trend.months[index];
            ImGui::BeginTooltip();
            ImGui::TextUnformatted(Ui::monthYear(month.month, month.year).c_str());
            ImGui::Separator();
            ImGui::TextColored(Theme::Savings, "Balance   %s", Ui::money(trend.actual[index]).c_str());
            ImGui::TextColored(Theme::Expected, "Expected  %s", Ui::money(trend.expected[index]).c_str());
            const double variance = trend.actual[index] - trend.expected[index];
            ImGui::TextColored(variance >= 0.0 ? Theme::Positive : Theme::Negative, "Variance  %s", Ui::signedMoney(variance).c_str());
            ImGui::EndTooltip();
        }
        ImPlot::EndPlot();
    }

}
