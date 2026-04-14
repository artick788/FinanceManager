#pragma once

#include <FinanceManager/Wage.hpp>
#include <VelyraUtils/VelyraEnum.hpp>

VL_ENUM(AppState, int,
    Overview = 0,
    ConfigureFinance = 1
);

namespace FinMgr {

    using namespace Velyra;

    class AddIncomePopup;
    class AddPaycheckPopup;

    struct Store {
        WageManager wageManager;

        AppState state = Overview;

        SP<AddIncomePopup> addIncomePopup;
        SP<AddPaycheckPopup> addPaycheckPopup;

        VL_GENERATE_JSON_SERIALIZER(Store, wageManager);
    };

    static const std::vector<std::string> s_ItemTypes = {
        "Income",
        "Savings",
        "Monthly Expenses"
    };
}