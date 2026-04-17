#include "SavingsOverview.hpp"

#include "Utils.hpp"

#include "Popups/AddSavingsEntryPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    SavingsOverview::SavingsOverview(App::AppData &appData, Store &store):
    m_AppData(appData),
    m_Store(store){

    }

    void SavingsOverview::drawStatistics() const {

    }

    void SavingsOverview::drawDetail() {
        auto& savings = m_Store.savingsManager.getSavingsRecords();
        if (savings.empty()) {
            ImGui::Text("No savings records added yet.");
            return;
        }
        ImGui::TextWrapped("Savings Records Count: %zu", savings.size());
        if (ImGui::BeginTabBar("SavingsTabs")) {
            for (auto& [name, record]: savings) {
                if (ImGui::BeginTabItem(name.c_str())) {
                    const std::string addEntryLabel = "Add Entry##" + name;
                    const std::string graphsLabel = "Graphs##" + name;

                    if (ImGui::Button(addEntryLabel.c_str())) {
                        m_Store.addSavingsEntryPopup->setSavingsName(name);
                        m_Store.addSavingsEntryPopup->setOpen(true);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(graphsLabel.c_str())) {
                        // TODO: Open graphs for this savings record
                    }

                    ImGui::Separator();
                    drawTable(name, record);
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    }

    void SavingsOverview::drawTable(const std::string &name, SavingsRecord &record) {
        auto& entries = record.getEntries();
        if (entries.empty()) {
            ImGui::Text("No entries added yet.");
            return;
        }
        const std::string tableLabel = "Entries##" + name;
        if (ImGui::BeginTable(tableLabel.c_str(), 7, ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn(("Year##" + name).c_str());
            ImGui::TableSetupColumn(("Month##" + name).c_str());
            ImGui::TableSetupColumn(("Day##" + name).c_str());
            ImGui::TableSetupColumn(("Transaction##" + name).c_str());
            ImGui::TableSetupColumn(("Expected##" + name).c_str());
            ImGui::TableSetupColumn(("Actual##" + name).c_str());
            ImGui::TableSetupColumn(("Variance##" + name).c_str());
            ImGui::TableHeadersRow();

            int currentYear = entries.begin()->date.year;
            int currentMonth = entries.begin()->date.month;
            double expected = record.initialAmount;

            // First row for initial amount
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);  ImGui::Text("Start");
            ImGui::TableSetColumnIndex(1);  ImGui::TextUnformatted("");
            ImGui::TableSetColumnIndex(2);  ImGui::TextUnformatted("");
            ImGui::TableSetColumnIndex(3);  ImGui::TextUnformatted("");
            ImGui::TableSetColumnIndex(4);  ImGui::Text("%.2f", expected);
            ImGui::TableSetColumnIndex(5);  ImGui::Text("%.2f", record.initialAmount);
            ImGui::TableSetColumnIndex(6);  ImGui::Text("%.2f", record.initialAmount - expected);

            bool drawYear = true;
            bool drawMonth = true;
            for (Size i = 0; i < entries.size(); ++i) {
                auto& entry = entries[i];

                ImGui::TableNextRow();
                if (entry.date.year > currentYear) {
                    drawTotalRow(expected, entry.actualState, App::Styles::ColorHighLight);
                    // Draw empty row
                    tableEmptyRow(7);

                    ImGui::TableNextRow(); // Same here, we inserted some extra rows

                    currentYear = entry.date.year;
                    currentMonth = entry.date.month; // Reset month when year changes
                    drawYear = true;
                    drawMonth = true;
                }
                if (entry.date.month > currentMonth) {
                    currentMonth = entry.date.month;
                    drawMonth = true;
                }

                ImGui::TableSetColumnIndex(0);
                if (drawYear) {
                    ImGui::Text("%d", entry.date.year);
                    drawYear = false;
                }
                else {
                    ImGui::TextUnformatted("");
                }
                ImGui::TableSetColumnIndex(1);
                if (drawMonth) {
                    ImGui::Text("%s", getMonthStr(entry.date.month));
                    drawMonth = false;
                }
                else {
                    ImGui::TextUnformatted("");
                }
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", entry.date.day);
                const std::string editTransactedAmountLabel = "##" + name + std::to_string(i) + "Transacted";
                ImGui::TableSetColumnIndex(3); ImGui::InputDouble(editTransactedAmountLabel.c_str(), &entry.expectedTransactionAmount, 0.0, 0.0, "%.2f");
                expected += entry.expectedTransactionAmount;
                ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", expected);

                const std::string editActualLabel = "##" + name + std::to_string(i);
                ImGui::TableSetColumnIndex(5); ImGui::InputDouble(editActualLabel.c_str(), &entry.actualState, 0.0, 0.0, "%.2f");
                const double variance = entry.actualState - expected;
                ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f (%.2f%%)", variance, expected != 0.0 ? (variance / expected) * 100.0 : 0.0);
            }
            drawTotalRow(expected, entries.back().actualState, App::Styles::ColorGreen);
            ImGui::EndTable();
        }
    }

    void SavingsOverview::drawTotalRow(const double expected, const double actual, const ImVec4 &color) {
        ImGui::TableNextRow();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(color));
        ImGui::TableSetColumnIndex(0); ImGui::Text("Total");
        ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted("");
        ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted("");
        ImGui::TableSetColumnIndex(3); ImGui::TextUnformatted("");
        ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", expected);
        ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", actual);
        const double variance = actual - expected;
        ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f, (%.2f%%)", variance, expected != 0.0 ? (variance / expected) * 100.0 : 0.0);
    }
}
