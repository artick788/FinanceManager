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

        void drawTotalRow(const std::vector<std::string>& expenseNames, const std::unordered_map<std::string, double>& totals, const ImVec4& color);

    private:
        App::AppData& m_AppData;
        Store& m_Store;

    };
}