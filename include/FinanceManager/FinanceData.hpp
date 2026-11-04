#pragma once

#include <FinanceManager/Wage.hpp>
#include <FinanceManager/Savings.hpp>
#include <FinanceManager/MonthlyExpenses.hpp>

#include <optional>

namespace FinMgr {

    /**
     * @brief All financial data of the user. The json layout is the on-disk format, so the field names must never
     *        change: {"wageManager": ..., "savingsManager": ..., "monthlyExpensesManager": ...}
     */
    struct FinanceData {
        WageManager wageManager;
        SavingsManager savingsManager;
        MonthlyExpensesManager monthlyExpensesManager;

        /**
         * @brief Sorts all entries and fills in missing monthly expenses up to today
         */
        void checkData();

        VL_GENERATE_JSON_SERIALIZER_RELAXED(FinanceData, wageManager, savingsManager, monthlyExpensesManager);
    };

    /**
     * @brief Where FinanceData was found in a settings file
     */
    enum class FinanceDataSource {
        None,   // No data yet, a fresh install
        Store,  // The "Store" key, used since VelyraAppFramework v0.4
        Legacy  // The "AppSettings"/"Store" key, used before VelyraAppFramework v0.4
    };

    struct FinanceDataLoadResult {
        FinanceData data;
        FinanceDataSource source = FinanceDataSource::None;
        std::string error; // Not empty if data was found but could not be parsed
    };

    /**
     * @brief Finds and parses the finance data in the given settings json. The "Store" key is preferred, if it is
     *        missing the data of the old framework ("AppSettings"/"Store") is used. Never throws.
     * @param storeJson The value of the "Store" key, std::nullopt if the key does not exist
     * @param legacyAppSettingsJson The value of the "AppSettings" key, std::nullopt if the key does not exist
     */
    FinanceDataLoadResult loadFinanceData(const std::optional<nlohmann::json>& storeJson,
                                          const std::optional<nlohmann::json>& legacyAppSettingsJson);

}
