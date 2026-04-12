#include <FinanceManager/Wage.hpp>

namespace FinMgr {

    Wage::Wage(const std::string& name) : m_Name(name) {
    }

    void Wage::addEntry(const WageEntry& entry) {
        m_Entries.push_back(entry);
        // sort entries by date
        std::ranges::sort(m_Entries, [](const WageEntry& a, const WageEntry& b) {
            return a.date < b.date;
        });
    }

    const std::string& Wage::getName() const {
        return m_Name;
    }

    const std::vector<WageEntry>& Wage::getEntries() const {
        return m_Entries;
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
}
