#include "Persistence.hpp"

#include <VelyraUtils/Clock/Clock.hpp>

namespace FinMgr {

    namespace {

        constexpr auto APP_NAME = "FinanceManager";

        // Saving to disk is cheap, but syncing with git is not, so only save automatically without git
        constexpr Duration AUTO_SAVE_DELAY = 20s;
        constexpr Duration DIRTY_CHECK_INTERVAL = 500ms;

        // Settings of this app that are specific to this PC, or only kept as a backup of the old format
        const std::vector<std::string> APP_LOCAL_KEYS = {
            App::SettingsKeys::IMGUI_SETTINGS,
            SettingKeys::UI,
            SettingKeys::LEGACY_APP_SETTINGS,
            SettingKeys::LEGACY_GIT_INFO,
            SettingKeys::LEGACY_LOCAL_SETTINGS
        };

        /**
         * @return true if a key was added
         */
        bool addAppLocalKeys(GitSyncDesc& gitSync) {
            bool added = false;
            for (const auto& key: APP_LOCAL_KEYS) {
                if (std::ranges::find(gitSync.localKeys, key) == gitSync.localKeys.end()) {
                    gitSync.localKeys.push_back(key);
                    added = true;
                }
            }
            return added;
        }

        std::optional<nlohmann::json> readJson(const App::Settings& settings, const std::string& key) {
            nlohmann::json json = settings.getSetting<nlohmann::json>(key);
            if (json.is_null()) {
                return std::nullopt;
            }
            return std::optional<nlohmann::json>(std::in_place, std::move(json));
        }

    }

    fs::path getSettingsFilePath() {
        // Same as the default of the framework, which fills in an empty SettingsDesc::settingsFile with this path
        return App::getAppdataPath(APP_NAME) / (std::string("settings_") + APP_NAME + ".json");
    }

    bool isGitSyncActive(const GitSyncDesc& gitSync) {
        return gitSync.enableGitSync && !gitSync.remoteUrl.empty();
    }

    // -----------------------------------------------------------------------------------------------------------------

    Persistence::Persistence(App::Settings& settings, Store& store):
    m_Settings(settings),
    m_Store(store) {

    }

    void Persistence::load() {
        // The app keeps some settings per PC, also when git sync was configured by hand in the settings file
        if (GitSyncDesc gitSync = getGitSync(); addAppLocalKeys(gitSync)) {
            m_Settings.setGitSync(gitSync);
        }
        m_Store.ui = m_Settings.getSetting<UiState>(SettingKeys::UI);
        loadData();
        if (m_Migrated) {
            backupBeforeMigration();
            // Write the data under its new key right away, and count it as unsaved so it reaches the disk soon
            commit();
            m_SavedData = nlohmann::json();
        }
    }

    void Persistence::loadData() {
        const FinanceDataLoadResult result = loadFinanceData(
            readJson(m_Settings, SettingKeys::STORE),
            readJson(m_Settings, SettingKeys::LEGACY_APP_SETTINGS));

        m_LoadError = result.error;
        m_Migrated = result.source == FinanceDataSource::Legacy && result.error.empty();
        m_Store.data = result.data;
        markSaved();
    }

    void Persistence::backupBeforeMigration() const {
        const fs::path settingsFile = getSettingsFilePath();
        const fs::path backup = settingsFile.parent_path() / (settingsFile.stem().string() + ".pre-v0.4-backup.json");
        std::error_code error;
        if (fs::exists(settingsFile, error) && !fs::exists(backup, error)) {
            fs::copy_file(settingsFile, backup, error);
        }
    }

    void Persistence::commit() {
        m_Settings.setSetting(SettingKeys::UI, m_Store.ui);
        if (isReadOnly()) {
            return;
        }
        m_Settings.setSetting(SettingKeys::STORE, m_Store.data);
    }

    void Persistence::save() {
        commit();
        m_Settings.save();
        if (!isReadOnly()) {
            markSaved();
        }
        m_StatusMessage = isGitSyncActive() ? "Saved and synced" : "Saved";
    }

    bool Persistence::syncNow() {
        commit();
        if (!m_Settings.sync()) {
            m_StatusMessage = isGitSyncActive() ? "Could not reach the git remote, changes are pushed on the next sync" : "Git sync is disabled";
            return false;
        }
        // The merge may have brought in changes from another PC
        loadData();
        m_StatusMessage = "Synced with git";
        return true;
    }

    void Persistence::onUpdate(const Duration deltaTime) {
        m_SinceCheck += deltaTime;
        if (m_SinceCheck >= DIRTY_CHECK_INTERVAL) {
            m_SinceCheck = Duration{};
            m_Dirty = !isReadOnly() && m_Store.data.toJson() != m_SavedData;
        }
        m_DirtyFor = m_Dirty ? m_DirtyFor + deltaTime : Duration{};
        if (m_DirtyFor >= AUTO_SAVE_DELAY && !isGitSyncActive()) {
            save();
            m_StatusMessage = "Saved automatically";
        }
    }

    void Persistence::setGitSync(GitSyncDesc gitSync) {
        addAppLocalKeys(gitSync);
        commit();
        m_Settings.setGitSync(gitSync);
        if (FinMgr::isGitSyncActive(gitSync)) {
            syncNow();
        }
        else {
            m_StatusMessage = "Git sync disabled";
        }
    }

    std::string Persistence::getLegacyRemoteUrl() const {
        const std::optional<nlohmann::json> legacy = readJson(m_Settings, SettingKeys::LEGACY_GIT_INFO);
        if (legacy.has_value() && legacy->is_object() && legacy->contains("sshString") && legacy->at("sshString").is_string()) {
            return legacy->at("sshString").get<std::string>();
        }
        return {};
    }

    void Persistence::markSaved() {
        m_SavedData = m_Store.data.toJson();
        m_Dirty = false;
        m_DirtyFor = Duration{};
    }

}
