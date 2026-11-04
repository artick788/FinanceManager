#pragma once

#include "Commands.hpp"
#include "Persistence.hpp"
#include "Store.hpp"

#include <VelyraAppFramework/Layer.hpp>

namespace FinMgr {

    class FinanceLayout;

    class FinanceManagerLayer: public App::Layer {
    public:
        explicit FinanceManagerLayer(App::AppData& appData);

        ~FinanceManagerLayer() override;

        void onAttach() override;

        void onDetach() override;

        void onUpdate(Duration deltaTime) override;

        void mainMenuBar() override;

    private:
        void attachPopups();

        void registerCommands();

        void drawStatus() const;

    private:
        // Declared before everything that keeps a reference to it
        Store m_Store;
        Persistence m_Persistence;
        CommandRegistry m_Commands;
        SP<FinanceLayout> m_Layout;
    };

}
