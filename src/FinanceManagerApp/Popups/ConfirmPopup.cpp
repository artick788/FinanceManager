#include "ConfirmPopup.hpp"

#include "../Theme.hpp"

#include <VelyraAppFramework/AppData.hpp>

namespace FinMgr {

    ConfirmPopup::ConfirmPopup(App::AppData& appData):
    Popup(appData, "Are you sure?", FINMGR_POPUP_CONFIRM) {

    }

    void ConfirmPopup::ask(const std::string& message, const std::string& confirmLabel, std::function<void()> action) {
        m_Message = message;
        m_ConfirmLabel = confirmLabel;
        m_Action = std::move(action);
        m_AppData.openPopup(getID());
    }

    void ConfirmPopup::drawContent() {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 26.0f);
        ImGui::TextUnformatted(m_Message.c_str());
        ImGui::PopTextWrapPos();
        ImGui::TextDisabled("This cannot be undone.");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Theme::Negative.x, Theme::Negative.y, Theme::Negative.z, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Negative);
        const bool confirmed = ImGui::Button(m_ConfirmLabel.c_str());
        ImGui::PopStyleColor(2);
        ImGui::SameLine();
        const bool cancelled = ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape);

        if (confirmed && m_Action) {
            // Copy first, the action may open this popup again
            const auto action = m_Action;
            reset();
            setOpen(false);
            action();
        }
        else if (cancelled) {
            reset();
            setOpen(false);
        }
    }

    void ConfirmPopup::reset() {
        m_Message.clear();
        m_ConfirmLabel.clear();
        m_Action = nullptr;
    }

}
