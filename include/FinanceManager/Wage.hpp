#pragma once

#include <FinanceManager/Date.hpp>

namespace FinMgr {

    struct WageEntry {
        Date date;
        double amount = 0.0;
        double extra = 0.0;

        VL_GENERATE_JSON_SERIALIZER(WageEntry, date, amount, extra);
    };

    struct WageStatistics {
        double totalCheck = 0.0;
        double totalExtra = 0.0;
        double totalAmount = 0.0;
    };

    class Wage {
    public:
        Wage() = default;

        explicit Wage(const std::string& name);

        ~Wage() = default;

        void addEntry(const WageEntry& entry);

        void checkData();

        void sortEntries();

        const std::string& getName() const;

        std::vector<WageEntry>& getEntries();

        const std::vector<WageEntry>& getEntries() const;

        /**
         * @brief Removes the given entry, which must point into getEntries()
         * @return false if the entry does not belong to this wage
         */
        bool removeEntry(const WageEntry* entry);

        WageStatistics calculateStatistics(const Date& min, const Date& max) const;

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

        std::unordered_map<std::string, Wage>& getWages();

        const std::unordered_map<std::string, Wage>& getWages() const;

        bool hasWage(const std::string& name) const;

        void checkData();

        WageStatistics calculateStatistics(const Date& min, const Date& max) const;

        VL_GENERATE_JSON_SERIALIZER(WageManager, m_Wages);

    private:
        std::unordered_map<std::string, Wage> m_Wages;
    };

}
