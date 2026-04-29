#include "TotalsLayout.hpp"

namespace FinMgr {

    TotalsLayout::TotalsLayout(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store) {

    }

    SP<App::Node> TotalsLayout::createLayout() const {
        using namespace Velyra::App;

        return App::createLayout(
            horizontalSplit(
                verticalSplit(
                    createPanel({.name = "Main", .sizeRatio = 0.4f}),
                    createPanel({.name = "Options", .sizeRatio = 0.6f})
                ),
                createPanel({.name = "Statistic", .sizeRatio = 0.7f})
            )
        );
    }

    void TotalsLayout::draw() {
        m_AppData.layoutEngine.beginPanel("Main");
        drawMain();
        m_AppData.layoutEngine.endPanel("Main");

        m_AppData.layoutEngine.beginPanel("Options");
        drawOptions();
        m_AppData.layoutEngine.endPanel("Options");

        m_AppData.layoutEngine.beginPanel("Statistic");
        drawStatistic();
        m_AppData.layoutEngine.endPanel("Statistic");
    }

    void TotalsLayout::drawMain() {
        if (ImGui::BeginListBox("##Items", ImVec2(-FLT_MIN, -FLT_MIN))) {
            for (int i = 0; i < s_ItemTypes.size(); ++i) {
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

    void TotalsLayout::drawOptions() {
        switch (m_SelectedItemType) {
            case 0:
                ImGui::Text("Options for Income");
                break;
            case 1:
                drawOptionsSavings();
                break;
            case 2:
                ImGui::Text("Options for Monthly Expenses");
                break;
            default:
                ImGui::Text("Select an item to see options");
        }
    }

    void TotalsLayout::drawOptionsSavings() {
        if (ImGui::Button("Show Savings Evolution")) {
            m_SelectedTotalsType = TotalSavingsEvolution;
        }
    }

    void TotalsLayout::drawStatistic() {
        switch (m_SelectedTotalsType) {
            case TotalSavingsEvolution:
                drawTotalSavingsEvolution();
                break;
            default:
                ImGui::Text("Select an option to see statistics");
        }
    }

    void TotalsLayout::drawTotalSavingsEvolution() {
        ImGui::Text("Total Savings Evolution");
    }
}
