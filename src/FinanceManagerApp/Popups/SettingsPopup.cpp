#include "SettingsPopup.hpp"

#include "../Theme.hpp"
#include "../Ui.hpp"

#include <VelyraAppFramework/AppData.hpp>

namespace FinMgr {

    SettingsPopup::SettingsPopup(App::AppData& appData, Store& store, Persistence& persistence):
    Popup(appData, "Settings", FINMGR_POPUP_SETTINGS),
    m_Store(store),
    m_Persistence(persistence) {

    }

    void SettingsPopup::open() {
        reset();
        m_AppData.openPopup(getID());
    }

    void SettingsPopup::drawContent() {
        ImGui::Dummy(ImVec2(520.0f * ImGui::GetStyle().FontScaleDpi, 0.0f));
        drawDisplaySettings();
        ImGui::Spacing();
        drawGitSettings();
        ImGui::Spacing();
        drawDataSettings();
        ImGui::Spacing();
        ImGui::Separator();
        if (ImGui::Button("Close") || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
            setOpen(false);
        }
    }

    void SettingsPopup::reset() {
        m_GitDraft = m_Persistence.getGitSync();
    }

    void SettingsPopup::drawDisplaySettings() {
        ImGui::SeparatorText("Display");
        Ui::formLabel("Currency symbol");
        ImGui::InputTextWithHint("##Currency", "e.g. EUR", &m_Store.ui.currency);
        Ui::formLabel("Dashboard range");
        const int options[] = {6, 12, 24, 36, 0};
        const char* labels[] = {"6 months", "12 months", "24 months", "36 months", "Everything"};
        int current = 1;
        for (int i = 0; i < 5; ++i) {
            if (options[i] == m_Store.ui.chartMonths) {
                current = i;
            }
        }
        if (ImGui::Combo("##Range", &current, labels, 5)) {
            m_Store.ui.chartMonths = options[current];
        }
    }

    void SettingsPopup::drawGitSettings() {
        ImGui::SeparatorText("Sync with git");
        ImGui::PushTextWrapPos(0.0f);
        ImGui::TextDisabled("Your data is pushed to a git repository when saving and pulled when the app starts, so it "
                            "stays the same on every PC. Changes made offline are merged on the next sync. This is stored "
                            "in the \"GitSync\" object of the settings file, which is never synced itself.");
        ImGui::PopTextWrapPos();

        ImGui::Checkbox("Enable git sync", &m_GitDraft.enableGitSync);
        ImGui::BeginDisabled(!m_GitDraft.enableGitSync);
        Ui::formLabel("Remote");
        ImGui::InputTextWithHint("##Remote", "git@github.com:you/settings.git", &m_GitDraft.remoteUrl);
        const std::string legacyRemote = m_Persistence.getLegacyRemoteUrl();
        if (m_GitDraft.remoteUrl.empty() && !legacyRemote.empty()) {
            const std::string label = "Use " + legacyRemote;
            if (ImGui::TextLink(label.c_str())) {
                m_GitDraft.remoteUrl = legacyRemote;
            }
            ImGui::SameLine();
            ImGui::TextDisabled("(from the previous version)");
        }
        Ui::formLabel("Branch");
        ImGui::InputTextWithHint("##Branch", "main", &m_GitDraft.branch);
        ImGui::EndDisabled();

        const GitSyncDesc active = m_Persistence.getGitSync();
        const bool changed = m_GitDraft.enableGitSync != active.enableGitSync || m_GitDraft.remoteUrl != active.remoteUrl || m_GitDraft.branch != active.branch;

        ImGui::BeginDisabled(!changed);
        if (ImGui::Button("Apply")) {
            m_Persistence.setGitSync(m_GitDraft);
            m_GitDraft = m_Persistence.getGitSync();
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        ImGui::BeginDisabled(changed || !isGitSyncActive(active));
        if (ImGui::Button("Sync now")) {
            m_Persistence.syncNow();
        }
        ImGui::EndDisabled();
        if (!m_Persistence.getStatusMessage().empty()) {
            ImGui::SameLine();
            ImGui::TextDisabled("%s", m_Persistence.getStatusMessage().c_str());
        }
    }

    void SettingsPopup::drawDataSettings() const {
        ImGui::SeparatorText("Data");
        ImGui::TextDisabled("Stored in");
        ImGui::SameLine();
        const std::string path = getSettingsFilePath().string();
        ImGui::TextUnformatted(path.c_str());
        if (ImGui::BeginItemTooltip()) {
            ImGui::TextUnformatted("Click to copy");
            ImGui::EndTooltip();
        }
        if (ImGui::IsItemClicked()) {
            ImGui::SetClipboardText(path.c_str());
        }
        if (m_Persistence.wasMigrated()) {
            ImGui::TextColored(Theme::Positive, "Imported from the previous version, the original file is kept as a backup next to it.");
        }
    }

}
