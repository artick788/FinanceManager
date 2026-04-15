#pragma once

#include "IncomeOverview.hpp"
#include "SavingsOverview.hpp"

namespace FinMgr {

    class OverviewLayout {
    public:
        OverviewLayout(App::AppData& appData, Store& store);

        ~OverviewLayout() = default;

        SP<App::Node> createLayout() const;

        void draw();

    private:

        void drawMain();

        void drawStatistics();

        void drawDetail();

    private:
        App::AppData& m_AppData;
        Store& m_Store;

        IncomeOverview m_IncomeOverview;
        SavingsOverview m_SavingsOverview;

        int m_SelectedItemType = -1;
    };
}
