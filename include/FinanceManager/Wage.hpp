#pragma once

#include <FinanceManager/Date.hpp>

namespace FinMgr {

    struct WageEntry {
        Date date;
        double amount = 0.0;
        double extra = 0.0;

        VL_GENERATE_JSON_SERIALIZER(WageEntry, date, amount, extra);
    };

    class Wage {
    public:
        Wage() = default;

        explicit Wage(const std::string& name);

        ~Wage() = default;

        void addEntry(const WageEntry& entry);

        const std::string& getName() const;

        const std::vector<WageEntry>& getEntries() const;

        VL_GENERATE_JSON_SERIALIZER(Wage, m_Name, m_Entries);

    private:
        std::string m_Name;
        std::vector<WageEntry> m_Entries;
    };

    class WageManager {
    public:
        WageManager() = default;

        ~WageManager() = default;

        void createWage(const std::string& name);

        void destroyWage(const std::string& name);

        Wage& getWage(const std::string& name);

        VL_GENERATE_JSON_SERIALIZER(WageManager, m_Wages);

    private:
        std::unordered_map<std::string, Wage> m_Wages;
    };

}
