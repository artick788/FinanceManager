#include "OverviewLayout.hpp"

#include "../Utils.hpp"

#include "../Popups/AddPaycheckPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    OverviewLayout::OverviewLayout(App::AppData& appData, Store& store) :
    Layout(FINMGR_APP_OVERVIEW_LAYOUT),
    m_AppData(appData),
    m_Store(store),
    m_IncomeOverview(appData, store),
    m_SavingsOverview(appData, store),
    m_MonthlyExpenseOverview(appData, store) {

    }

    SP<App::Node> OverviewLayout::getLayout() {
        using namespace Velyra::App;

        return App::createLayout(
            horizontalSplit(
                verticalSplit(
                    createPanel({.name = "Main", .drawFunction = bindDraw(this, &OverviewLayout::drawMain), .sizeRatio = 0.4f}),
                    createPanel({.name = "Statistics", .drawFunction = bindDraw(this, &OverviewLayout::drawStatistics), .sizeRatio = 0.6f})
                ),
                createPanel({.name = "Detail", .drawFunction = bindDraw(this, &OverviewLayout::drawDetail), .sizeRatio = 0.7f})
            )
        );
    }

    void OverviewLayout::drawMain(Core::Window& window, Core::Context& context) {
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

    void OverviewLayout::drawStatistics(Core::Window& window, Core::Context& context) {
        switch (m_SelectedItemType) {
            case 0:
                m_IncomeOverview.drawStatistics();
                break;
            case 1:
                m_SavingsOverview.drawStatistics();
                break;
            case 2:
                m_MonthlyExpenseOverview.drawStatistics();
                break;
            default:
                ImGui::Text("Select an item type to view statistics.");
                break;
        }
    }

    void OverviewLayout::drawDetail(Core::Window& window, Core::Context& context) {
        switch (m_SelectedItemType) {
            case 0:
                m_IncomeOverview.drawDetail();
                break;
            case 1:
                m_SavingsOverview.drawDetail();
                break;
            case 2:
                m_MonthlyExpenseOverview.drawDetail();
                break;
            default:
                ImGui::Text("Select an item type to view details.");
                break;
        }
    }

}
