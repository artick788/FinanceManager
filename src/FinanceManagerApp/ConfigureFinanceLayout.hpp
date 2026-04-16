#pragma once

#include "Store.hpp"
#include <VelyraAppFramework/Application.hpp>

namespace FinMgr {

    class ConfigureFinanceLayout {
    public:
        ConfigureFinanceLayout(App::AppData& appData, Store& store);

        ~ConfigureFinanceLayout() = default;

        SP<App::Node> createLayout() const;

        void draw();

    private:

        void drawItems();

        void drawItemDetail() const;

        void drawIncomeDetail() const;

        void drawSavingsDetail() const;

        void drawMonthlyExpensesDetail() const;

    private:
        App::AppData& m_AppData;
        Store& m_Store;

        int m_SelectedItemType = -1;
    };
}
