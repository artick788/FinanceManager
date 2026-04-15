#include "ConfigureFinanceLayout.hpp"

#include "Popups/CreateIncomePopup.hpp"
#include "Popups/CreateSavingsRecordPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    ConfigureFinanceLayout::ConfigureFinanceLayout(App::AppData& appData,Store &store):
    m_AppData(appData),
    m_Store(store){

    }

    SP<App::Node> ConfigureFinanceLayout::createLayout() const {
        using namespace Velyra::App;

        return App::createLayout(
            horizontalSplit(
                createPanel({.name = "Items", .sizeRatio = 0.2f}),
                createPanel({.name = "ItemDetail", .sizeRatio = 0.8f})
            )
        );
    }

    void ConfigureFinanceLayout::draw() {
        m_AppData.layoutEngine.beginPanel("Items");
        drawItems();
        m_AppData.layoutEngine.endPanel("Items");

        m_AppData.layoutEngine.beginPanel("ItemDetail");
        drawItemDetail();
        m_AppData.layoutEngine.endPanel("ItemDetail");
    }

    void ConfigureFinanceLayout::drawItems() {
        if (ImGui::BeginListBox("##Items", ImVec2(-FLT_MIN, -FLT_MIN))) {
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

    void ConfigureFinanceLayout::drawItemDetail() {
        switch (m_SelectedItemType) {
            case 0:
                drawIncomeDetail();
                break;
            case 1:
                drawSavingsDetail();
                break;
            case 2:
                // drawMonthlyExpensesDetail();
                break;
            default:
                ImGui::Text("Select an item to see details");
                break;
        }
    }

    void ConfigureFinanceLayout::drawIncomeDetail() {
        ImGui::Text("Income");
        if (ImGui::Button("Add Income")) {
            m_Store.addIncomePopup->setOpen(true);
        }
        ImGui::Separator();

        auto& wages = m_Store.wageManager.getWages();
        if (wages.empty()) {
            ImGui::Text("No income sources added yet.");
            return;
        }
        std::string toRemove;
        if (ImGui::BeginTable("Income", 2, ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Remove");
            ImGui::TableHeadersRow();

            for (const auto &name: wages | std::views::keys) {
                const std::string buttonLabel = "Remove##" + name;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", name.c_str());
                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
                if (ImGui::Button(buttonLabel.c_str())) {
                    toRemove = name;
                }
                ImGui::PopStyleColor();
            }
            ImGui::EndTable();
        }
        if (!toRemove.empty()) {
            m_Store.wageManager.destroyWage(toRemove);
        }
    }

    void ConfigureFinanceLayout::drawSavingsDetail() {
        ImGui::Text("Savings");
        if (ImGui::Button("Add Savings Record")) {
            m_Store.createSavingsRecordPopup->setOpen(true);
        }
        ImGui::Separator();
        auto& savings = m_Store.savingsManager.getSavingsRecords();
        if (savings.empty()) {
            ImGui::Text("No savings records added yet.");
            return;
        }
        std::string toRemove;
        if (ImGui::BeginTable("Savings", 4, ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Initial Amount");
            ImGui::TableSetupColumn("Default Transaction Amount");
            ImGui::TableSetupColumn("Remove");
            ImGui::TableHeadersRow();

            for (auto &[name, record]: savings) {
                const std::string buttonLabel = "Remove##" + name;

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", name.c_str());
                ImGui::TableSetColumnIndex(1);
                const std::string editInitialAmountLabel = "EditInitial##" + name;
                ImGui::InputDouble(editInitialAmountLabel.c_str(), &record.initialAmount);
                ImGui::TableSetColumnIndex(2);
                const std::string editDefaultTransactionAmountLabel = "EditDefault##" + name;
                ImGui::InputDouble(editDefaultTransactionAmountLabel.c_str(), &record.defaultTransactionAmount, 0.01, 0.0, "%.2f");
                ImGui::TableSetColumnIndex(3);
                ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
                if (ImGui::Button(buttonLabel.c_str())) {
                    toRemove = name;
                }
                ImGui::PopStyleColor();
            }
            ImGui::EndTable();
        }
        if (!toRemove.empty()) {
            m_Store.savingsManager.destroySavingsRecord(toRemove);
        }
    }
}
