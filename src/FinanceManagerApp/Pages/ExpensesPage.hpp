#pragma once

#include "../Store.hpp"

#include <FinanceManager/Reports.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace FinMgr {

    class ExpensesPage {
    public:
        explicit ExpensesPage(Store& store);

        SP<App::Node> buildLayout();

    private:
        void drawTable();

        void drawChart();

        // One row per kind of row in the table, the table itself is built by buildExpenseTable()
        void drawTotalRow(const char* yearLabel, const char* monthLabel, const std::vector<double>& totals, double total, const ImVec4& background);

        void drawMonthRow(const ExpenseTable& table, const ExpenseMonthRow& row);

        void drawCell(const std::string& expenseName, const ExpenseCell& cell, YearMonth month);

        void drawSpacerRow();

    private:
        Store& m_Store;
        YearMonth m_Today;
    };

}
