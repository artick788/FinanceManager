#include "CreateIncomePopup.hpp"

#include <VelyraAppFramework/Styles.hpp>
#include <VelyraCore/ImGui/ImGui.hpp>

#include "../Utils.hpp"

namespace FinMgr {

    CreateIncomePopup::CreateIncomePopup(App::AppData& appData, Store& store):
    Popup(appData, "Add Income", FINMGR_APP_POPUP_CREATE_INCOME),
    m_Store(store) {

    }

    void CreateIncomePopup::drawContent(Core::Window&, Core::Context&) {
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

    void CreateIncomePopup::reset() {
        m_WageName.clear();
    }
}
