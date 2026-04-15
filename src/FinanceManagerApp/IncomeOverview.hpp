#pragma once

#include "Store.hpp"
#include <VelyraAppFramework/Application.hpp>

namespace FinMgr {

    class IncomeOverview {
    public:
        IncomeOverview(App::AppData& appData, Store& store);

        ~IncomeOverview() = default;

        void drawStatistics() const;

        void drawDetail();

    private:

        void drawIncomeTable(const std::string& name, Wage& wage);

        void drawIncomeTableTotalRow(double check, double extra, double total, const ImVec4& color);

    private:
        App::AppData& m_AppData;
        Store& m_Store;

    };
}