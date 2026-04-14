#include "AddIncomePopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    AddIncomePopup::AddIncomePopup(Store& store):
    Popup("Add Income"),
    m_Store(store) {

    }

    void AddIncomePopup::drawContent() {
        ImGui::InputTextWithHint("##WageName", "Enter wage name", &m_WageName);

        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            if (!m_WageName.empty() && !m_Store.wageManager.hasWage(m_WageName)) {
                m_Store.wageManager.createWage(m_WageName);
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

    void AddIncomePopup::reset() {
        m_WageName.clear();
    }
}
