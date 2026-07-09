#pragma once

#include "../IncomeOverview.hpp"
#include "../SavingsOverview.hpp"
#include "../MonthlyExpenseOverview.hpp"

#include <VelyraAppFramework/LayoutEngine/Layout.hpp>

namespace FinMgr {

    class OverviewLayout: public App::Layout {
    public:
        OverviewLayout(App::AppData& appData, Store& store);

        ~OverviewLayout() override = default;

        SP<App::Node> getLayout() override;

    private:

        void drawMain(Core::Window& window, Core::Context& context);

        void drawStatistics(Core::Window& window, Core::Context& context);

        void drawDetail(Core::Window& window, Core::Context& context);

    private:
        App::AppData& m_AppData;
        Store& m_Store;

        IncomeOverview m_IncomeOverview;
        SavingsOverview m_SavingsOverview;
        MonthlyExpenseOverview m_MonthlyExpenseOverview;

        int m_SelectedItemType = -1;
    };
}
