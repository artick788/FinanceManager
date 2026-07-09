#pragma once

#include <VelyraAppFramework/Widgets/Panel.hpp>
#include "../Store.hpp"

namespace FinMgr {

    class ExpenseTrendPanel: public App::Widgets::Panel {
    public:
        ExpenseTrendPanel(App::AppData& appData, Store& store);

        ~ExpenseTrendPanel() override = default;

    protected:
        void drawContent(Core::Window&, Core::Context&) override;

    private:
        void createGraphData(Store& store);

        void drawDetails();

        void drawGraph();

    private:
        Store& m_Store;

        std::vector<double> m_Times;
        std::vector<double> m_Values;
    };
}