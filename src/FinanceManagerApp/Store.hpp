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

    struct Store {
        WageManager wageManager;

        AppState state = ConfigureFinance;

        SP<AddIncomePopup> addIncomePopup;

        VL_GENERATE_JSON_SERIALIZER(Store, wageManager);
    };
}