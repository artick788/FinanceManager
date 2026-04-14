#include "FinanceManagerLayer.hpp"

#include "Popups/AddIncomePopup.hpp"

namespace FinMgr {

    FinanceManagerLayer::FinanceManagerLayer(App::AppData& app_data) : App::Layer(app_data) {
    }

    void FinanceManagerLayer::onAttach(const UP<Core::Window>& window, const UP<Core::Context>& context) {
        loadSettings();

        m_ConfigureFinanceLayout = createUP<ConfigureFinanceLayout>(m_AppData, m_Store);
        m_OverviewLayout = createUP<OverviewLayout>(m_AppData, m_Store);

        setLayout();
        attachPopups();

    }

    void FinanceManagerLayer::onDetach(const UP<Core::Window> &, const UP<Core::Context> &) {
        saveSettings();
    }

    void FinanceManagerLayer::onUpdate(const UP<Core::Window> &, const UP<Core::Context> &, Duration) {
        m_AppData.layoutEngine.setActiveLayout(Utils::toString(m_Store.state));
    }

    void FinanceManagerLayer::onImGui(const UP<Core::Window>& window, const UP<Core::Context>& context) {
        drawMainMenuBar();

        switch (m_Store.state) {
            case Overview: {
                m_OverviewLayout->draw();
                break;
            }
            case ConfigureFinance: {
                m_ConfigureFinanceLayout->draw();
                break;
            }
            default:
                return;
        }
    }

    void FinanceManagerLayer::setLayout() const {
        m_AppData.layoutEngine.registerLayout(Utils::toString(Overview), m_OverviewLayout->createLayout());
        m_AppData.layoutEngine.registerLayout(Utils::toString(ConfigureFinance), m_ConfigureFinanceLayout->createLayout());
    }

    void FinanceManagerLayer::loadSettings() {
        m_Store = m_AppData.settings.getSetting<Store>("Store");
    }

    void FinanceManagerLayer::saveSettings() {
        m_AppData.settings.setSetting("Store", m_Store);
    }

    void FinanceManagerLayer::attachPopups() {
        m_Store.addIncomePopup = createSP<AddIncomePopup>(m_Store);
        m_AppData.addPopup(m_Store.addIncomePopup);
    }

    void FinanceManagerLayer::createPanels() {

    }

    void FinanceManagerLayer::drawMainMenuBar() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Overview")) {
                    m_Store.state = Overview;
                }
                if (ImGui::MenuItem("Finance Configuration")) {
                    m_Store.state = ConfigureFinance;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

    }
}
