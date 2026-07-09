#include "AddSavingsEntryPopup.hpp"

#include "../Widgets/DateInput.hpp"
#include "../Utils.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    AddSavingsEntryPopup::AddSavingsEntryPopup(App::AppData& appData, Store& store):
    Popup(appData, "Add Savings Entry", FINMGR_APP_POPUP_ADD_SAVINGS_ENTRY),
    m_Store(store) {

    }

    void AddSavingsEntryPopup::setSavingsName(const std::string& savingsName) {
        m_SavingsName = savingsName;
        const auto& record = m_Store.savingsManager.getSavingsRecord(m_SavingsName);
        m_InputAmount = record.defaultTransactionAmount;
    }

    void AddSavingsEntryPopup::drawContent(Core::Window&, Core::Context&) {
        Widgets::DateInput(m_InputDate, true);
        ImGui::InputDouble("Amount", &m_InputAmount, 0.01, 0.0, "%.2f");
        ImGui::InputDouble("Current State", &m_InputCurrentState, 0.01, 0.0, "%.2f");

        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            m_Store.savingsManager.getSavingsRecord(m_SavingsName).addEntry(m_InputDate, m_InputAmount, m_InputCurrentState);
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
    }

    void AddSavingsEntryPopup::reset() {
        m_InputAmount = 0.0;
        m_InputCurrentState = 0.0;
        m_InputDate = getCurrentDate();
    }

}