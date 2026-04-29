#include "FinanceManagerLayer.hpp"

#include "Popups/CreateIncomePopup.hpp"
#include "Popups/AddPaycheckPopup.hpp"
#include "Popups/CreateSavingsRecordPopup.hpp"
#include "Popups/AddSavingsEntryPopup.hpp"
#include "Popups/CreateMonthlyExpensePopup.hpp"
#include "Popups/AddMonthlyExpensePopup.hpp"

namespace FinMgr {

    FinanceManagerLayer::FinanceManagerLayer(App::AppData& app_data) : App::Layer(app_data) {
    }

    void FinanceManagerLayer::onAttach(const UP<Core::Window>& window, const UP<Core::Context>& context) {
        loadSettings();

        m_ConfigureFinanceLayout = createUP<ConfigureFinanceLayout>(m_AppData, m_Store);
        m_OverviewLayout = createUP<OverviewLayout>(m_AppData, m_Store);
        m_TotalsLayout = createUP<TotalsLayout>(m_AppData, m_Store);

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
            case Totals: {
                m_TotalsLayout->draw();
                break;
            }
            default:
                return;
        }
    }

    void FinanceManagerLayer::setLayout() const {
        m_AppData.layoutEngine.registerLayout(Utils::toString(Overview), m_OverviewLayout->createLayout());
        m_AppData.layoutEngine.registerLayout(Utils::toString(ConfigureFinance), m_ConfigureFinanceLayout->createLayout());
        m_AppData.layoutEngine.registerLayout(Utils::toString(Totals), m_TotalsLayout->createLayout());
    }

    void FinanceManagerLayer::loadSettings() {
        m_Store = m_AppData.settings.getSetting<Store>("Store");
        m_Store.monthlyExpensesManager.checkData();
        m_Store.wageManager.checkData();
        m_Store.savingsManager.checkData();
    }

    void FinanceManagerLayer::saveSettings() {
        m_AppData.settings.setSetting("Store", m_Store);
    }

    void FinanceManagerLayer::attachPopups() {
        m_Store.addIncomePopup = createSP<CreateIncomePopup>(m_Store);
        m_AppData.addPopup(m_Store.addIncomePopup);
        m_Store.addPaycheckPopup = createSP<AddPaycheckPopup>(m_Store);
        m_AppData.addPopup(m_Store.addPaycheckPopup);
        m_Store.createSavingsRecordPopup = createSP<CreateSavingsRecordPopup>(m_Store);
        m_AppData.addPopup(m_Store.createSavingsRecordPopup);
        m_Store.addSavingsEntryPopup = createSP<AddSavingsEntryPopup>(m_Store);
        m_AppData.addPopup(m_Store.addSavingsEntryPopup);
        m_Store.createMonthlyExpensePopup = createSP<CreateMonthlyExpensePopup>(m_Store);
        m_AppData.addPopup(m_Store.createMonthlyExpensePopup);
        m_Store.addMonthlyExpensePopup = createSP<AddMonthlyExpensePopup>(m_Store);
        m_AppData.addPopup(m_Store.addMonthlyExpensePopup);

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
                if (ImGui::MenuItem("Totals")) {
                    m_Store.state = Totals;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

    }
}
