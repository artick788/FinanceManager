#pragma once

#include <VelyraAppFramework/Application.hpp>

#include "Store.hpp"
#include "OverviewLayout.hpp"
#include "ConfigureFinanceLayout.hpp"

namespace FinMgr {
    
    class FinanceManagerLayer : public App::Layer {
    public:
        explicit FinanceManagerLayer(App::AppData& app_data);

        void onAttach(const UP<Core::Window>& window, const UP<Core::Context>& context) override;

        void onDetach(const UP<Core::Window> &, const UP<Core::Context> &) override;

        void onUpdate(const UP<Core::Window> &, const UP<Core::Context> &, Duration) override;

        void onImGui(const UP<Core::Window>& window, const UP<Core::Context>& context) override;

    private:
        void setLayout() const;

        void loadSettings();

        void saveSettings();

        void attachPopups();

        void createPanels();

        void drawMainMenuBar();

    private:
        Store m_Store;

        UP<OverviewLayout> m_OverviewLayout;
        UP<ConfigureFinanceLayout> m_ConfigureFinanceLayout;
    };

}
