#include <FinanceManager/FinanceData.hpp>

namespace FinMgr {

    void FinanceData::checkData() {
        wageManager.checkData();
        savingsManager.checkData();
        monthlyExpensesManager.checkData();
    }

    namespace {

        bool hasData(const std::optional<nlohmann::json>& json) {
            return json.has_value() && json->is_object() && !json->empty();
        }

    }

    FinanceDataLoadResult loadFinanceData(const std::optional<nlohmann::json>& storeJson,
                                          const std::optional<nlohmann::json>& legacyAppSettingsJson) {
        FinanceDataLoadResult result;

        const nlohmann::json* source = nullptr;
        if (hasData(storeJson)) {
            source = &storeJson.value();
            result.source = FinanceDataSource::Store;
        }
        else if (legacyAppSettingsJson.has_value() && legacyAppSettingsJson->is_object() &&
                 legacyAppSettingsJson->contains("Store") && legacyAppSettingsJson->at("Store").is_object()) {
            source = &legacyAppSettingsJson->at("Store");
            result.source = FinanceDataSource::Legacy;
        }

        if (source == nullptr) {
            return result;
        }
        try {
            result.data.fromJson(*source);
            result.data.checkData();
        }
        catch (const std::exception& e) {
            result.data = FinanceData{};
            result.error = e.what();
        }
        return result;
    }

}
