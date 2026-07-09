#pragma once

#include "../Store.hpp"
#include <VelyraAppFramework/LayoutEngine/Layout.hpp>

namespace Velyra{
    namespace App {
        class AppData;
    }
    namespace Core {
        class Window;
        class Context;
    }
}

namespace FinMgr {

    class ConfigureFinanceLayout: public App::Layout {
    public:
        ConfigureFinanceLayout(App::AppData& appData, Store& store);

        ~ConfigureFinanceLayout() override = default;

        SP<App::Node> getLayout() override;

    private:

        void drawItems(Core::Window& window, Core::Context& context);

        void drawItemDetail(Core::Window& window, Core::Context& context);

        void drawIncomeDetail() const;

        void drawSavingsDetail() const;

        void drawMonthlyExpensesDetail() const;

    private:
        App::AppData& m_AppData;
        Store& m_Store;

        int m_SelectedItemType = -1;
    };
}
