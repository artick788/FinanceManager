#pragma once

#include "Store.hpp"
#include <VelyraAppFramework/Application.hpp>

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

        void drawIncomeDetail();

        void drawIncomeTable(const std::string& name, Wage& wage);

        void drawIncomeTableTotalRow(double check, double extra, double total, const ImVec4& color);

    private:
        App::AppData& m_AppData;
        Store& m_Store;

        int m_SelectedItemType = -1;
    };
}
