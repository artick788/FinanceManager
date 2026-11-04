#pragma once

#include "ConfirmPopup.hpp"
#include "../Persistence.hpp"

namespace FinMgr {

    class SettingsPopup: public App::Widgets::Popup {
    public:
        SettingsPopup(App::AppData& appData, Store& store, Persistence& persistence);

        void open();

    protected:
        void drawContent() override;

        void reset() override;

    private:
        void drawDisplaySettings();

        void drawGitSettings();

        void drawDataSettings() const;

    private:
        Store& m_Store;
        Persistence& m_Persistence;

        GitSyncConfig m_GitDraft;
    };

}
