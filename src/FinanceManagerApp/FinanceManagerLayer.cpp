#include "FinanceManagerLayer.hpp"

namespace FinMgr {

    FinanceManagerLayer::FinanceManagerLayer(App::AppData& app_data) : App::Layer(app_data) {
    }

    void FinanceManagerLayer::onAttach(const UP<Core::Window>& window, const UP<Core::Context>& context) {
        setLayout();

        m_Store = m_AppData.settings.getSetting<Store>("Store");
    }

    void FinanceManagerLayer::onDetach(const UP<Core::Window> &, const UP<Core::Context> &) {
        m_AppData.settings.setSetting("Store", m_Store);
    }

    void FinanceManagerLayer::onImGui(const UP<Core::Window>& window, const UP<Core::Context>& context) {

    }

    void FinanceManagerLayer::setLayout() const {
        using namespace Velyra::App;

        auto layout = createLayout(
        horizontalSplit(
                verticalSplit(
                    createPanel({.name = "Main", .sizeRatio = 0.6f}),
                    createPanel({.name = "Statistics", .sizeRatio = 0.4f})
                ),
                createPanel({.name = "Detail", .sizeRatio = 0.6f})
            )
        );

        m_AppData.layoutEngine.registerLayout("DEFAULT", layout);
        m_AppData.layoutEngine.setActiveLayout("DEFAULT");
    }
}
