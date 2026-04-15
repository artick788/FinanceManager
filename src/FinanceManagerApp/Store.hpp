#pragma once

#include <FinanceManager/Wage.hpp>
#include <FinanceManager/Savings.hpp>
#include <VelyraUtils/VelyraEnum.hpp>

VL_ENUM(AppState, int,
    Overview = 0,
    ConfigureFinance = 1
);

namespace FinMgr {

    using namespace Velyra;

    class AddIncomePopup;
    class AddPaycheckPopup;
    class CreateSavingsRecordPopup;

    struct Store {
        WageManager wageManager;
        SavingsManager savingsManager;

        AppState state = Overview;

        SP<AddIncomePopup> addIncomePopup;
        SP<AddPaycheckPopup> addPaycheckPopup;
        SP<CreateSavingsRecordPopup> createSavingsRecordPopup;

        // TODO: Would it be handy to also store the app state? It could save on click in the UI?
        VL_GENERATE_JSON_SERIALIZER(Store, wageManager, savingsManager);
    };

    static const std::vector<std::string> s_ItemTypes = {
        "Income",
        "Savings",
        "Monthly Expenses"
    };
}