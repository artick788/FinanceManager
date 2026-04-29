#pragma once

#include <VelyraAppFramework/Application.hpp>
#include "Store.hpp"

VL_ENUM(TotalsType, int,
    TotalSavingsEvolution
);

namespace FinMgr {

    class TotalsLayout {
    public:
        TotalsLayout(App::AppData& appData, Store& store);

        ~TotalsLayout() = default;

        SP<App::Node> createLayout() const;

        void draw();

    private:

        void drawMain();

        void drawOptions();

        void drawOptionsSavings();

        void drawStatistic();

        void drawTotalSavingsEvolution();

    private:
        App::AppData& m_AppData;
        Store& m_Store;

        int m_SelectedItemType = -1;
        TotalsType m_SelectedTotalsType = TotalsType_MAX_VALUE;
    };

}