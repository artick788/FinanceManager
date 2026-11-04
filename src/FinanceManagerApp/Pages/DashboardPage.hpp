#pragma once

#include "../Store.hpp"

#include <FinanceManager/Reports.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace FinMgr {

    class DashboardPage {
    public:
        explicit DashboardPage(Store& store);

        SP<App::Node> buildLayout();

    private:
        void drawOverview();

        void drawCashFlow();

        void drawSavings();

        void drawFixedCosts();

        bool drawRangeSelector();

        /**
         * @return The first month shown in the charts, based on the selected range
         */
        YearMonth getChartStart() const;

    private:
        Store& m_Store;

        // Built once per frame in buildLayout(), shared by the panels
        YearMonth m_Today;
        std::optional<YearMonth> m_FirstMonth;
        CashFlow m_CashFlow;
    };

}
