#pragma once

#include "Store.hpp"
#include <VelyraAppFramework/Application.hpp>

namespace FinMgr {

    class MonthlyExpenseOverview {
    public:
        MonthlyExpenseOverview(App::AppData& appData, Store& store);

        ~MonthlyExpenseOverview() = default;

        void drawStatistics() const;

        void drawDetail();

    private:

        void drawTable();

    private:
        App::AppData& m_AppData;
        Store& m_Store;

    };
}