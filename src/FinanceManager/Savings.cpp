#include <FinanceManager/Savings.hpp>

namespace FinMgr {

    SavingsRecord::SavingsRecord(const std::string &name, const double initialAmount_):
    initialAmount(initialAmount_),
    m_Name(name){

    }

    void SavingsRecord::addEntry(const Date &date, const double expectedTransactionAmount, const double actualState) {
        m_Entries.emplace_back(SavingEntry{date, expectedTransactionAmount, actualState});
        sortEntries();
    }

    void SavingsRecord::checkData() {
        sortEntries();
    }

    void SavingsRecord::sortEntries() {
        // Sort entries by date
        std::ranges::sort(m_Entries, [](const SavingEntry& a, const SavingEntry& b) {
            return a.date < b.date;
        });
    }

    bool SavingsRecord::removeEntry(const SavingEntry* entry) {
        return eraseElement(m_Entries, entry);
    }

    SavingsStatistics SavingsRecord::getMonthlySavedAmount(const Date &date) const {
        SavingsStatistics stats;
        stats.expectedAmount = initialAmount;
        stats.actualAmount = 0.0;
        for (const auto& entry: m_Entries) {
            if (entry.date <= date) {
                stats.expectedAmount += entry.expectedTransactionAmount;
            }
            if (entry.date.year == date.year && entry.date.month == date.month) {
                stats.actualAmount = entry.actualState;
                return stats;
            }
        }
        return stats;
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

    const std::unordered_map<std::string, SavingsRecord>& SavingsManager::getSavingsRecords() const {
        return m_SavingsRecords;
    }

    bool SavingsManager::hasSavingsRecord(const std::string &name) const {
        return m_SavingsRecords.contains(name);
    }

    void SavingsManager::checkData() {
        for (auto &record: m_SavingsRecords | std::views::values) {
            record.checkData();
        }
    }

    TotalSavingsTrend SavingsManager::getTotalSavingsTrend() const {
        TotalSavingsTrend trend;

        Date earliestDate = getEarliestSavingsRecordDate();
        const Date currentDate = getCurrentDate();
        for (; earliestDate <= currentDate; earliestDate.monthIncrement()) {
            trend.dates.push_back(earliestDate.toUnixTimestamp());

            double totalActualSavedAmount = 0.0;
            double totalExpectedSavedAmount = 0.0;
            for (const auto& record: m_SavingsRecords | std::views::values) {
                auto [expectedAmount, actualAmount] = record.getMonthlySavedAmount(earliestDate);
                totalActualSavedAmount += actualAmount;
                totalExpectedSavedAmount += expectedAmount;
            }
            trend.actualAmounts.push_back(totalActualSavedAmount);
            trend.expectedAmounts.push_back(totalExpectedSavedAmount);
        }
        return trend;
    }

    Date SavingsManager::getEarliestSavingsRecordDate() const {
        Date earliestDate = Date(31, 12, 9999); // I will probably never live longer than the year 9999
        for (const auto& record: m_SavingsRecords | std::views::values) {
            if (!record.getEntries().empty()) {
                const Date& recordEarliestDate = record.getEntries().front().date;
                if (recordEarliestDate < earliestDate) {
                    earliestDate = recordEarliestDate;
                }
            }
        }
        return earliestDate;
    }
}
