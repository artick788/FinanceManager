#pragma once

#include <VelyraAppFramework/Widgets/Panel.hpp>
#include "../Store.hpp"

namespace FinMgr {

    class TotalSavingsTrendPanel : public App::Widgets::Panel {
    public:
        TotalSavingsTrendPanel(App::AppData& appData, const Store& store);

        ~TotalSavingsTrendPanel() override = default;

    protected:
        void drawContent(Core::Window&, Core::Context&) override;

    private:
        void drawDetails();

        void drawGraph();

        void createTrendData();

    private:
        TotalSavingsTrend m_Trend;

        std::array<double, 2> m_TimeRange;
        std::array<double, 2> m_TrendPoints;
        double m_AvgMonthlyChange = 0.0;

    };
}
