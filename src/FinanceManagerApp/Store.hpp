#pragma once

#include <FinanceManager/Wage.hpp>

namespace FinMgr {

    using namespace Velyra;

    struct Store {
        WageManager wageManager;

        VL_GENERATE_JSON_SERIALIZER(Store, wageManager);
    };
}