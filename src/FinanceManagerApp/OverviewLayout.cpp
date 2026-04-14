#include "OverviewLayout.hpp"

#include "Utils.hpp"

#include "Popups/AddPaycheckPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    OverviewLayout::OverviewLayout(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store) {

    }

    SP<App::Node> OverviewLayout::createLayout() const {
        using namespace Velyra::App;

        return App::createLayout(
            horizontalSplit(
                verticalSplit(
                    createPanel({.name = "Main", .sizeRatio = 0.4f}),
                    createPanel({.name = "Statistics", .sizeRatio = 0.6f})
                ),
                createPanel({.name = "Detail", .sizeRatio = 0.7f})
            )
        );
    }

    void OverviewLayout::draw() {
        m_AppData.layoutEngine.beginPanel("Main");
        drawMain();
        m_AppData.layoutEngine.endPanel("Main");

        m_AppData.layoutEngine.beginPanel("Statistics");
        drawStatistics();
        m_AppData.layoutEngine.endPanel("Statistics");

        m_AppData.layoutEngine.beginPanel("Detail");
        drawDetail();
        m_AppData.layoutEngine.endPanel("Detail");
    }

    void OverviewLayout::drawMain() {
        if (ImGui::BeginListBox("##ItemsMain", ImVec2(-FLT_MIN, -FLT_MIN))) {
            for (size_t i = 0; i < s_ItemTypes.size(); ++i) {
                const bool is_selected = m_SelectedItemType == i;
                if (ImGui::Selectable(s_ItemTypes[i].c_str(), is_selected)) {
                    m_SelectedItemType = i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }

    void OverviewLayout::drawStatistics() {
        switch (m_SelectedItemType) {
            case 0:
                ImGui::Text("Income statistics will be displayed here.");
                break;
            case 1:
                ImGui::Text("Savings statistics will be displayed here.");
                break;
            case 2:
                ImGui::Text("Monthly expenses statistics will be displayed here.");
                break;
            default:
                ImGui::Text("Select an item type to view statistics.");
                break;
        }
    }

    void OverviewLayout::drawDetail() {
        switch (m_SelectedItemType) {
            case 0:
                drawIncomeDetail();
                break;
            case 1:
                ImGui::Text("Savings details will be displayed here.");
                break;
            case 2:
                ImGui::Text("Monthly expenses details will be displayed here.");
                break;
            default:
                ImGui::Text("Select an item type to view details.");
                break;
        }
    }

    void OverviewLayout::drawIncomeDetail() {
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

    void OverviewLayout::drawIncomeTable(const std::string& name, Wage &wage) {
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
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("");
                }
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s", getMonthStr(entry.date.month));
                ImGui::TableSetColumnIndex(2); ImGui::Text("%d", entry.date.day);

                const std::string editCheckLabel = "Edit##" + name + std::to_string(i);
                const std::string editExtraLabel = "Edit##" + name + std::to_string(i) + "Extra";
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

    void OverviewLayout::drawIncomeTableTotalRow(const double check, const double extra, const double total, const ImVec4 &color) {
        ImGui::TableNextRow();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(color));
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Total:");
        ImGui::TableSetColumnIndex(1); ImGui::Text("");
        ImGui::TableSetColumnIndex(2); ImGui::Text("");
        ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", check);
        ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", extra);
        ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", total);
    }
}
