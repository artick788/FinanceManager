#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class CreateMonthlyExpensePopup : public App::Widgets::Popup {
    public:
        explicit CreateMonthlyExpensePopup(Store& store);

        ~CreateMonthlyExpensePopup() override = default;

    protected:

        void drawContent() override;

        void reset() override;

    private:
        Store& m_Store;

        std::string m_MonthlyExpenseName;
        int m_InputDefaultTransactionDay = 1;
        double m_InputDefaultTransactionAmount = 0.0;
    };

}