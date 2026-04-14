#include "OverviewLayout.hpp"

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
                    createPanel({.name = "Main", .sizeRatio = 0.6f}),
                    createPanel({.name = "Statistics", .sizeRatio = 0.4f})
                ),
                createPanel({.name = "Detail", .sizeRatio = 0.4f})
            )
        );
    }

    void OverviewLayout::draw() {
        m_AppData.layoutEngine.beginPanel("Main");
        ImGui::Text("Welcome to the Finance Manager!");
        ImGui::Text("Use the panels on the right to view statistics and details.");
        m_AppData.layoutEngine.endPanel("Main");

        m_AppData.layoutEngine.beginPanel("Statistics");
        ImGui::Text("Statistics will be displayed here.");
        m_AppData.layoutEngine.endPanel("Statistics");

        m_AppData.layoutEngine.beginPanel("Detail");
        ImGui::Text("Detailed information will be displayed here.");
        m_AppData.layoutEngine.endPanel("Detail");
    }

}
