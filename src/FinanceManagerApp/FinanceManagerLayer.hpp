#pragma once

#include <VelyraAppFramework/Application.hpp>

#include "Store.hpp"
#include "Layouts/OverviewLayout.hpp"
#include "Layouts/ConfigureFinanceLayout.hpp"

namespace FinMgr {
    
    class FinanceManagerLayer : public App::Layer {
    public:
        explicit FinanceManagerLayer(App::AppData& app_data);

        ~FinanceManagerLayer() override;

        void mainMenuBar(Core::Window &window, Core::Context &context) override;

    private:

        void loadSettings();

        void saveSettings();

        void attachPopups();

    private:
        Store m_Store;

        OverviewLayout m_OverviewLayout;
        ConfigureFinanceLayout m_ConfigureFinanceLayout;
    };

}
