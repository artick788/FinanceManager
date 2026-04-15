#include "OverviewLayout.hpp"

#include "Utils.hpp"

#include "Popups/AddPaycheckPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    OverviewLayout::OverviewLayout(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store),
    m_IncomeOverview(appData, store),
    m_SavingsOverview(appData, store) {

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
                m_IncomeOverview.drawStatistics();
                break;
            case 1:
                m_SavingsOverview.drawStatistics();
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
                m_IncomeOverview.drawDetail();
                break;
            case 1:
                m_SavingsOverview.drawDetail();
                break;
            case 2:
                ImGui::Text("Monthly expenses details will be displayed here.");
                break;
            default:
                ImGui::Text("Select an item type to view details.");
                break;
        }
    }

}
