#include "AddPaycheckPopup.hpp"

#include "../Widgets/DateInput.hpp"
#include "../Utils.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    AddPaycheckPopup::AddPaycheckPopup(App::AppData& appData, Store& store):
    Popup(appData, "Add Paycheck", FINMGR_APP_POPUP_ADD_PAYCHECK),
    m_Store(store) {

    }

    void AddPaycheckPopup::setWageName(const std::string &wageName) {
        m_WageName = wageName;
    }

    void AddPaycheckPopup::drawContent(Core::Window&, Core::Context&) {
        Widgets::DateInput(m_InputDate, true);
        ImGui::InputDouble("Amount", &m_InputCheck, 0.01, 1.0, "%.2f");
        ImGui::InputDouble("Extra", &m_InputExtra, 0.01, 1.0, "%.2f");

        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            if (m_InputCheck > 0.0) {
                const WageEntry entry{m_InputDate, m_InputCheck, m_InputExtra};
                m_Store.wageManager.getWage(m_WageName).addEntry(entry);
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

    void AddPaycheckPopup::reset() {
        m_InputCheck = 0.0;
        m_InputExtra = 0.0;
        m_InputDate = getCurrentDate();
    }
}
