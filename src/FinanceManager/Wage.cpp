#include <FinanceManager/Wage.hpp>

namespace FinMgr {

    Wage::Wage(const std::string& name) : m_Name(name) {
    }

    void Wage::addEntry(const WageEntry& entry) {
        m_Entries.push_back(entry);
        sortEntries();
    }

    void Wage::checkData() {
        sortEntries();
    }

    void Wage::sortEntries() {
        // Sort entries by date
        std::ranges::sort(m_Entries, [](const WageEntry& a, const WageEntry& b) {
            return a.date < b.date;
        });
    }

    const std::string& Wage::getName() const {
        return m_Name;
    }

    std::vector<WageEntry>& Wage::getEntries() {
        return m_Entries;
    }

    WageStatistics Wage::calculateStatistics(const Date& min, const Date& max) const {
        WageStatistics stats;
        for (const auto&[date, amount, extra] : m_Entries) {
            if (date >= min && date <= max) {
                stats.totalCheck += amount;
                stats.totalExtra += extra;
                stats.totalAmount += amount + extra;
            }
        }
        return stats;
    }

    void WageManager::createWage(const std::string &name) {
        m_Wages.emplace(name, Wage(name));
    }

    void WageManager::destroyWage(const std::string &name) {
        m_Wages.erase(name);
    }

    Wage& WageManager::getWage(const std::string &name) {
        return m_Wages.at(name);
    }

    std::unordered_map<std::string, Wage>& WageManager::getWages() {
        return m_Wages;
    }

    bool WageManager::hasWage(const std::string &name) const {
        return m_Wages.contains(name);
    }

    WageStatistics WageManager::calculateStatistics(const Date &min, const Date &max) const {
        WageStatistics stats;
        for (const auto &wage: m_Wages | std::views::values) {
            auto [totalCheck, totalExtra, totalAmount] = wage.calculateStatistics(min, max);
            stats.totalCheck += totalCheck;
            stats.totalExtra += totalExtra;
            stats.totalAmount += totalAmount;
        }
        return stats;
    }

    void WageManager::checkData() {
        for (auto &wage: m_Wages | std::views::values) {
            wage.checkData();
        }
    }
}
