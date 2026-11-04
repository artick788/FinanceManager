#pragma once

#include "Persistence.hpp"
#include "Pages/DashboardPage.hpp"
#include "Pages/IncomePage.hpp"
#include "Pages/SavingsPage.hpp"
#include "Pages/ExpensesPage.hpp"
#include "Pages/ManagePage.hpp"

#include <VelyraAppFramework/Layout.hpp>

namespace FinMgr {

    /**
     * @brief The layout tree is rebuilt every frame from the UI state: a navigator on the left, the panels of the
     *        active page on the right and a notice on top when something needs attention.
     *
     *        The tree is built before any panel draws, while drawing (e.g. clicking in the navigator) may change the
     *        state the tree was built from. Draw functions must therefore check their state again instead of assuming
     *        it still matches the tree, the tree catches up on the next frame.
     */
    class FinanceLayout: public App::Layout {
    public:
        FinanceLayout(Store& store, Persistence& persistence);

        ~FinanceLayout() override = default;

        SP<App::Node> getLayout() override;

    private:
        SP<App::Node> buildPage();

        bool hasNotice() const;

        void drawNotice();

        void drawNavigator();

        void drawNavigatorItem(Page page, const char* shortcut);

        void drawNavigatorFooter();

    private:
        Store& m_Store;
        Persistence& m_Persistence;

        DashboardPage m_Dashboard;
        IncomePage m_Income;
        SavingsPage m_Savings;
        ExpensesPage m_Expenses;
        ManagePage m_Manage;

        bool m_MigrationNoticeDismissed = false;
    };

}
