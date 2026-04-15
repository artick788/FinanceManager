#include "CreateSavingsRecordPopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    CreateSavingsRecordPopup::CreateSavingsRecordPopup(Store& store):
    Popup("Create Savings Record"),
    m_Store(store) {

    }

    void CreateSavingsRecordPopup::drawContent() {
        ImGui::InputTextWithHint("##RecordName", "Enter record name", &m_RecordName);
        ImGui::InputDouble("Default Transaction Amount", &m_InputDefaultTransactionAmount, 0.01, 0.0, "%.2f");
        ImGui::InputDouble("Initial Amount", &m_InputInitialAmount, 0.01, 0.0, "%.2f");

        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            if (!m_RecordName.empty() && !m_Store.savingsManager.hasSavingsRecord(m_RecordName)) {
                m_Store.savingsManager.createSavingsRecord(m_RecordName, m_InputInitialAmount, m_InputDefaultTransactionAmount);
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

    void CreateSavingsRecordPopup::reset() {
        m_RecordName.clear();
        m_InputDefaultTransactionAmount = 0.0;
        m_InputInitialAmount = 0.0;
    }

}
