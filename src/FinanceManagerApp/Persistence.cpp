#include "Persistence.hpp"

#include <VelyraUtils/Clock/Clock.hpp>

#include <fstream>
#include <iostream>

namespace FinMgr {

    namespace {

        constexpr auto APP_NAME = "FinanceManager";

        // Saving to disk is cheap, but syncing with git is not, so only save automatically without git
        constexpr Duration AUTO_SAVE_DELAY = 20s;
        constexpr Duration DIRTY_CHECK_INTERVAL = 500ms;

        /**
         * @brief Reads a setting as plain json. Utils::fromJson<nlohmann::json> recurses forever (json looks like a
         *        map to it), so the json is wrapped in a type with its own fromJson.
         */
        struct RawJson {
            nlohmann::json value;

            void fromJson(const nlohmann::json& json) { value = json; }

            nlohmann::json toJson() const { return value; }
        };

        std::optional<nlohmann::json> readRaw(const App::Settings& settings, const std::string& key) {
            nlohmann::json json = settings.getSetting<RawJson>(key).value;
            if (json.is_null()) {
                return std::nullopt;
            }
            return std::optional<nlohmann::json>(std::in_place, std::move(json));
        }

    }

    fs::path getSettingsFilePath() {
        return App::getAppdataPath(APP_NAME) / (std::string("settings_") + APP_NAME + ".json");
    }

    GitSyncConfig readGitSyncConfig(const fs::path& settingsFile) {
        std::ifstream file(settingsFile);
        if (!file.is_open()) {
            return {};
        }
        const nlohmann::json json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded() || !json.is_object() || !json.contains(SettingKeys::GIT_SYNC)) {
            return {};
        }
        try {
            return Utils::fromJson<GitSyncConfig>(json.at(SettingKeys::GIT_SYNC));
        }
        catch (const std::exception& e) {
            std::cerr << "Ignoring invalid git sync config: " << e.what() << std::endl;
            return {};
        }
    }

    App::SettingsDesc createSettingsDesc(const GitSyncConfig& config) {
        App::SettingsDesc desc;
        desc.enableSave = true;
        desc.settingsFile = getSettingsFilePath();
        // Everything that is specific to this PC, or only kept as a backup of the old format, is never synced
        desc.gitSync.localKeys = {
            "WindowSettings", "ContextSettings", "ImGuiSettings",
            SettingKeys::UI, SettingKeys::GIT_SYNC,
            SettingKeys::LEGACY_APP_SETTINGS, SettingKeys::LEGACY_GIT_INFO, SettingKeys::LEGACY_LOCAL_SETTINGS
        };
        if (config.isActive()) {
            desc.gitSync.remoteUrl = config.remoteUrl;
            desc.gitSync.branch = config.branch.empty() ? "main" : config.branch;
        }
        return desc;
    }

    // -----------------------------------------------------------------------------------------------------------------

    Persistence::Persistence(App::Settings& settings, Store& store):
    m_Settings(settings),
    m_Store(store) {

    }

    void Persistence::load() {
        m_GitSync = m_Settings.getSetting<GitSyncConfig>(SettingKeys::GIT_SYNC);
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
            readRaw(m_Settings, SettingKeys::STORE),
            readRaw(m_Settings, SettingKeys::LEGACY_APP_SETTINGS));

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
        m_Settings.setSetting(SettingKeys::GIT_SYNC, m_GitSync);
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
        m_StatusMessage = m_GitSync.isActive() ? "Saved and synced" : "Saved";
    }

    bool Persistence::syncNow() {
        commit();
        if (!m_Settings.sync()) {
            m_StatusMessage = m_GitSync.isActive() ? "Could not reach the git remote, changes are pushed on the next sync" : "Git sync is disabled";
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
        if (m_DirtyFor >= AUTO_SAVE_DELAY && !m_GitSync.isActive()) {
            save();
            m_StatusMessage = "Saved automatically";
        }
    }

    void Persistence::setGitSyncConfig(const GitSyncConfig& config) {
        m_GitSync = config;
        m_Settings.setGitSync(createSettingsDesc(config).gitSync);
        commit();
    }

    std::string Persistence::getLegacyRemoteUrl() const {
        const std::optional<nlohmann::json> legacy = readRaw(m_Settings, SettingKeys::LEGACY_GIT_INFO);
        if (legacy.has_value() && legacy->is_object() && legacy->contains("sshString") && legacy->at("sshString").is_string()) {
            return legacy->at("sshString").get<std::string>();
        }
        return {};
    }

    void Persistence::markSaved() {
        m_SavedData = m_Store.data.toJson();
        m_Dirty = false;
        m_DirtyFor = Duration{};
        m_LastSave = getTime();
    }

}
