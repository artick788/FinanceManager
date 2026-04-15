#pragma once

#include "Store.hpp"
#include <VelyraAppFramework/Application.hpp>

namespace FinMgr {

    class SavingsOverview {
    public:
        SavingsOverview(App::AppData& appData, Store& store);

        ~SavingsOverview() = default;

        void drawStatistics() const;

        void drawDetail();

    private:

        void drawTable(const std::string& name, SavingsRecord& record);

        void drawTotalRow(double expected, double actual, const ImVec4& color);

    private:
        App::AppData& m_AppData;
        Store& m_Store;

    };
}