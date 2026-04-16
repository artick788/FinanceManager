#pragma once

#include <FinanceManager/Date.hpp>

namespace FinMgr {

    struct SavingEntry {
        Date date;
        double expectedTransactionAmount = 0.0;
        double actualState = 0.0;

        VL_GENERATE_JSON_SERIALIZER(SavingEntry, date, expectedTransactionAmount, actualState);
    };

    /*
     * The expected amount is simply the initial value + the sum of all expected transactions up to the current date.
     * The actual amount is the the current amount of the savings at the given date
     */
    struct SavingsStatistics {
        double expectedAmount = 0.0;
        double actualAmount = 0.0;
    };

    class SavingsRecord {
    public:
        double defaultTransactionAmount = 0.0;
        double initialAmount = 0.0;

    public:
        SavingsRecord() = default;

        SavingsRecord(const std::string& name, double initialAmount_);

        void addEntry(const Date& date, double expectedTransactionAmount, double actualState);

        void checkData();

        void sortEntries();

        std::vector<SavingEntry>& getEntries() { return m_Entries; }

        const std::vector<SavingEntry>& getEntries() const { return m_Entries; }

        VL_GENERATE_JSON_SERIALIZER(SavingsRecord, m_Name, initialAmount, m_Entries, defaultTransactionAmount);

    private:
        std::string m_Name;
        std::vector<SavingEntry> m_Entries;
    };

    class SavingsManager {
    public:
        SavingsManager() = default;

        ~SavingsManager() = default;

        void createSavingsRecord(const std::string& name, double initialAmount, double defaultTransactionAmount = 0.0);

        void destroySavingsRecord(const std::string& name);

        SavingsRecord& getSavingsRecord(const std::string& name);

        std::unordered_map<std::string, SavingsRecord>& getSavingsRecords();

        bool hasSavingsRecord(const std::string& name) const;

        void checkData();

        VL_GENERATE_JSON_SERIALIZER(SavingsManager, m_SavingsRecords);

    private:
        std::unordered_map<std::string, SavingsRecord> m_SavingsRecords;

    };

}
