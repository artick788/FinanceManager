#include <FinanceManager/Savings.hpp>

namespace FinMgr {

    SavingsRecord::SavingsRecord(const std::string &name, const double initialAmount):
    m_Name(name),
    initialAmount(initialAmount) {

    }

    void SavingsRecord::addEntry(const Date &date, const double expectedTransactionAmount, const double actualState) {
        m_Entries.emplace_back(SavingEntry{date, expectedTransactionAmount, actualState});
    }

    void SavingsManager::createSavingsRecord(const std::string &name, const double initialAmount, const double defaultTransactionAmount) {
        m_SavingsRecords.emplace(name, SavingsRecord(name, initialAmount));
        m_SavingsRecords.at(name).defaultTransactionAmount = defaultTransactionAmount;
    }

    void SavingsManager::destroySavingsRecord(const std::string &name) {
        m_SavingsRecords.erase(name);
    }

    SavingsRecord& SavingsManager::getSavingsRecord(const std::string &name) {
        return m_SavingsRecords.at(name);
    }

    std::unordered_map<std::string, SavingsRecord>& SavingsManager::getSavingsRecords() {
        return m_SavingsRecords;
    }

    bool SavingsManager::hasSavingsRecord(const std::string &name) const {
        return m_SavingsRecords.contains(name);
    }
}
