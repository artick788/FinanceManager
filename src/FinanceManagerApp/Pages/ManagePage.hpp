#pragma once

#include "../Store.hpp"

#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace FinMgr {

    /**
     * @brief Create, configure and delete income sources, savings accounts and monthly expenses
     */
    class ManagePage {
    public:
        explicit ManagePage(Store& store);

        SP<App::Node> buildLayout();

    private:
        void drawIncome();

        void drawSavings();

        void drawExpenses();

        void askDelete(const std::string& what, const std::string& name, Size entryCount, std::function<void()> action);

    private:
        Store& m_Store;
    };

}
