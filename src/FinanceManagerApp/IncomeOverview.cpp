#include "IncomeOverview.hpp"

#include "Utils.hpp"

#include "Popups/AddPaycheckPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    IncomeOverview::IncomeOverview(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store) {

    }

    void IncomeOverview::drawStatistics() const {
        const Date now = getCurrentDate();
        const WageStatistics totalStats = m_Store.wageManager.calculateStatistics(Date{1, 1, 1}, now);
        const Date thisYearStart{1, 1, now.year};
        const WageStatistics thisYearStats = m_Store.wageManager.calculateStatistics(thisYearStart, now);
        const Date thisMonthStart{1, now.month, now.year};
        const WageStatistics thisMonthStats = m_Store.wageManager.calculateStatistics(thisMonthStart, now);

        ImGui::TextWrapped("Total Income: %.2f", totalStats.totalAmount);
        ImGui::TextWrapped("This Year: %.2f", thisYearStats.totalAmount);
        ImGui::TextWrapped("This Month: %.2f", thisMonthStats.totalAmount);
    }

    void IncomeOverview::drawDetail() {
        auto& wages = m_Store.wageManager.getWages();
        if (wages.empty()) {
            ImGui::Text("No income sources added yet.");
            return;
        }
        ImGui::TextWrapped("Income Count: %zu", wages.size());
        if (ImGui::BeginTabBar("IncomeTabs")) {
            for (auto& [name, wage]: wages) {
                if (ImGui::BeginTabItem(name.c_str())) {
                    const std::string addPayCheckLabel = "Add Paycheck##" + name;
                    const std::string graphsLabel = "Graphs##" + name;

                    if (ImGui::Button(addPayCheckLabel.c_str())) {
                        m_Store.addPaycheckPopup->setWageName(name);
                        m_Store.addPaycheckPopup->setOpen(true);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(graphsLabel.c_str())) {
                        // TODO: Open graphs for this wage
                    }

                    ImGui::Separator();
                    drawIncomeTable(name, wage);
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }

    void IncomeOverview::drawIncomeTable(const std::string& name, Wage &wage) {
        auto& entries = wage.getEntries();
        if (entries.empty()) {
            ImGui::Text("No paychecks added yet.");
            return;
        }
        const std::string tableLabel = "Paychecks##" + name;
        if (ImGui::BeginTable(tableLabel.c_str(), 6, ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn(("Year##" + name).c_str());
            ImGui::TableSetupColumn(("Month##" + name).c_str());
            ImGui::TableSetupColumn(("Day##" + name).c_str());
            ImGui::TableSetupColumn(("Check##" + name).c_str());
            ImGui::TableSetupColumn(("Extra##" + name).c_str());
            ImGui::TableSetupColumn(("Total##" + name).c_str());
            ImGui::TableHeadersRow();

            int currentYear = entries.begin()->date.year;
            double totalCheck = 0.0;
            double totalExtra = 0.0;
            double totalAmount = 0.0;
            double yearlyCheck = 0.0;
            double yearlyExtra = 0.0;
            double yearlyAmount = 0.0;

            bool drawYear = true;
            for (Size i = 0; i < entries.size(); ++i) {
                auto& entry = entries[i];

                ImGui::TableNextRow();
                if (entry.date.year > currentYear) {
                    drawIncomeTableTotalRow(yearlyCheck, yearlyExtra, yearlyAmount, App::Styles::ColorHighLight);

                    // Draw empty row
                    tableEmptyRow(6);
                    // Update totals
                    currentYear = entry.date.year;
                    yearlyCheck = 0.0;
                    yearlyExtra = 0.0;
                    yearlyAmount = 0.0;
                    drawYear = true; // Draw year in the next row
                    ImGui::TableNextRow(); // We have inserted 2 rows, so we need to call TableNextRow() twice to move to the next row for the new entry
                }
                ImGui::TableSetColumnIndex(0);
                if (drawYear) {
                    ImGui::Text("%d", entry.date.year);
                    drawYear = false;
                }
                else {
                    ImGui::TextUnformatted("");
                }
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", getMonthStr(entry.date.month));
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", entry.date.day);

                const std::string editCheckLabel = "##" + name + std::to_string(i);
                const std::string editExtraLabel = "##" + name + std::to_string(i) + "Extra";
                ImGui::TableSetColumnIndex(3); ImGui::InputDouble(editCheckLabel.c_str(), &entry.amount, 0.0, 0.0, "%.2f");
                ImGui::TableSetColumnIndex(4); ImGui::InputDouble(editExtraLabel.c_str(), &entry.extra, 0.0, 0.0, "%.2f");
                double total = entry.amount + entry.extra;
                ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", total);
                yearlyCheck += entry.amount;
                yearlyExtra += entry.extra;
                yearlyAmount += total;
                totalCheck += entry.amount;
                totalExtra += entry.extra;
                totalAmount += total;
            }
            // Draw final yearly total
            drawIncomeTableTotalRow(yearlyCheck, yearlyExtra, yearlyAmount, App::Styles::ColorHighLight);
            drawIncomeTableTotalRow(totalCheck, totalExtra, totalAmount, App::Styles::ColorGreen);
            ImGui::EndTable();
        }
    }

    void IncomeOverview::drawIncomeTableTotalRow(const double check, const double extra, const double total, const ImVec4 &color) {
        ImGui::TableNextRow();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(color));
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Total:");
        ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted("");
        ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted("");
        ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", check);
        ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", extra);
        ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", total);
    }
}
