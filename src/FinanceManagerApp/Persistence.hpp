#pragma once

#include "Store.hpp"

#include <VelyraAppFramework/Settings.hpp>

namespace FinMgr {

    namespace SettingKeys {
        constexpr auto STORE = "Store";                     // FinanceData, synced with git
        constexpr auto UI = "FinanceManagerUI";             // UiState, local to this PC
        constexpr auto GIT_SYNC = "FinanceManagerGitSync";  // GitSyncConfig, local to this PC

        // Written by the app before VelyraAppFramework v0.4, only read to migrate
        constexpr auto LEGACY_APP_SETTINGS = "AppSettings";
        constexpr auto LEGACY_GIT_INFO = "GitInfo";
        constexpr auto LEGACY_LOCAL_SETTINGS = "LocalSettings";
    }

    struct GitSyncConfig {
        bool enabled = false;
        std::string remoteUrl;
        std::string branch = "main";

        bool isActive() const { return enabled && !remoteUrl.empty(); }

        VL_GENERATE_JSON_SERIALIZER_RELAXED(GitSyncConfig, enabled, remoteUrl, branch)
    };

    /**
     * @return The settings file of the app, the same file the app used before VelyraAppFramework v0.4
     */
    fs::path getSettingsFilePath();

    /**
     * @brief Reads the git sync config straight from the settings file, needed before the application (and its
     *        Settings) exist, since git sync is configured when the settings are created.
     */
    GitSyncConfig readGitSyncConfig(const fs::path& settingsFile);

    App::SettingsDesc createSettingsDesc(const GitSyncConfig& config);

    /**
     * @brief Moves the finance data between the Store and the Settings. The data is only written back to the settings
     *        if it was read successfully, so a file that cannot be parsed is never overwritten with empty data.
     */
    class Persistence {
    public:
        Persistence(App::Settings& settings, Store& store);

        void load();

        /**
         * @brief Writes the store into the settings, without saving them to disk
         */
        void commit();

        /**
         * @brief Commits and saves to disk, syncing with git if enabled
         */
        void save();

        /**
         * @brief Commits, syncs with git and reloads the store with the merged result
         * @return false if git sync is disabled or the remote could not be reached
         */
        bool syncNow();

        /**
         * @brief Tracks unsaved changes and saves automatically when git sync is disabled
         */
        void onUpdate(Duration deltaTime);

        void setGitSyncConfig(const GitSyncConfig& config);

        const GitSyncConfig& getGitSyncConfig() const { return m_GitSync; }

        /**
         * @return The remote of the git info of the old framework, to make setting up git sync easier
         */
        std::string getLegacyRemoteUrl() const;

        bool hasUnsavedChanges() const { return m_Dirty; }

        /**
         * @return true if the data could not be read, saving the data is disabled to protect the file
         */
        bool isReadOnly() const { return !m_LoadError.empty(); }

        const std::string& getLoadError() const { return m_LoadError; }

        bool wasMigrated() const { return m_Migrated; }

        const std::string& getStatusMessage() const { return m_StatusMessage; }

        TimePoint getLastSaveTime() const { return m_LastSave; }

    private:
        void loadData();

        void backupBeforeMigration() const;

        void markSaved();

    private:
        App::Settings& m_Settings;
        Store& m_Store;

        GitSyncConfig m_GitSync;
        nlohmann::json m_SavedData;
        std::string m_LoadError;
        std::string m_StatusMessage;
        bool m_Migrated = false;
        bool m_Dirty = false;

        Duration m_SinceCheck{};
        Duration m_DirtyFor{};
        TimePoint m_LastSave = getTime();
    };

}
