#include "AddSavingsEntryPopup.hpp"

#include "../Widgets/DateInput.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    AddSavingsEntryPopup::AddSavingsEntryPopup(Store& store):
    Popup("Add Savings Entry"),
    m_Store(store) {

    }

    void AddSavingsEntryPopup::drawContent() {
        Widgets::DateInput(m_InputDate, true);
        ImGui::InputDouble("Amount", &m_InputAmount, 0.01, 0.0, "%.2f");
        ImGui::InputDouble("Current State", &m_InputCurrentState, 0.01, 0.0, "%.2f");

        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            if (m_InputAmount > 0.0) {
                m_Store.savingsManager.getSavingsRecord(m_SavingsName).addEntry(m_InputDate, m_InputAmount, m_InputCurrentState);
                reset();
                setOpen(false);
            }
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