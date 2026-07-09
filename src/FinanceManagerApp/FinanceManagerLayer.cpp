#include "FinanceManagerLayer.hpp"

#include "Popups/CreateIncomePopup.hpp"
#include "Popups/AddPaycheckPopup.hpp"
#include "Popups/CreateSavingsRecordPopup.hpp"
#include "Popups/AddSavingsEntryPopup.hpp"
#include "Popups/CreateMonthlyExpensePopup.hpp"
#include "Popups/AddMonthlyExpensePopup.hpp"

#include <VelyraCore/ImGui/ImGui.hpp>

namespace FinMgr {

    FinanceManagerLayer::FinanceManagerLayer(App::AppData& app_data) :
    App::Layer(app_data),
    m_ConfigureFinanceLayout(app_data, m_Store),
    m_OverviewLayout(app_data, m_Store) {
        loadSettings();
        attachPopups();

        m_AppData.registerLayout(m_OverviewLayout);
        m_AppData.registerLayout(m_ConfigureFinanceLayout);
        m_AppData.setActiveLayout(m_OverviewLayout.getID());
    }

    FinanceManagerLayer::~FinanceManagerLayer() {
        saveSettings();
    }

    void FinanceManagerLayer::mainMenuBar(Core::Window &window, Core::Context &context) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Overview")) {
                    m_AppData.setActiveLayout(m_OverviewLayout.getID());
                }
                if (ImGui::MenuItem("Finance Configuration")) {
                    m_AppData.setActiveLayout(m_ConfigureFinanceLayout.getID());
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
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
        m_Store.addIncomePopup = createSP<CreateIncomePopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_Store.addIncomePopup);
        m_Store.addPaycheckPopup = createSP<AddPaycheckPopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_Store.addPaycheckPopup);
        m_Store.createSavingsRecordPopup = createSP<CreateSavingsRecordPopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_Store.createSavingsRecordPopup);
        m_Store.addSavingsEntryPopup = createSP<AddSavingsEntryPopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_Store.addSavingsEntryPopup);
        m_Store.createMonthlyExpensePopup = createSP<CreateMonthlyExpensePopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_Store.createMonthlyExpensePopup);
        m_Store.addMonthlyExpensePopup = createSP<AddMonthlyExpensePopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_Store.addMonthlyExpensePopup);
    }
}
