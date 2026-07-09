#pragma once

#include <FinanceManager/Wage.hpp>
#include <FinanceManager/Savings.hpp>
#include <FinanceManager/MonthlyExpenses.hpp>

namespace FinMgr {

    using namespace Velyra;

    class CreateIncomePopup;
    class AddPaycheckPopup;
    class CreateSavingsRecordPopup;
    class AddSavingsEntryPopup;
    class CreateMonthlyExpensePopup;
    class AddMonthlyExpensePopup;

    struct Store {
        WageManager wageManager;
        SavingsManager savingsManager;
        MonthlyExpensesManager monthlyExpensesManager;


        SP<CreateIncomePopup> addIncomePopup;
        SP<AddPaycheckPopup> addPaycheckPopup;
        SP<CreateSavingsRecordPopup> createSavingsRecordPopup;
        SP<AddSavingsEntryPopup> addSavingsEntryPopup;
        SP<CreateMonthlyExpensePopup> createMonthlyExpensePopup;
        SP<AddMonthlyExpensePopup> addMonthlyExpensePopup;

        // TODO: Would it be handy to also store the app state? It could save on click in the UI?
        VL_GENERATE_JSON_SERIALIZER_RELAXED(Store, wageManager, savingsManager, monthlyExpensesManager);
    };

    static const std::vector<std::string> s_ItemTypes = {
        "Income",
        "Savings",
        "Monthly Expenses"
    };
}