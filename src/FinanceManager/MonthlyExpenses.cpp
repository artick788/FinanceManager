#include <FinanceManager/MonthlyExpenses.hpp>

namespace FinMgr {

    MonthlyExpense::MonthlyExpense(const std::string &name):
    m_Name(name){

    }

    void MonthlyExpense::addEntry(const Date &date, const double amount) {
        m_Entries.emplace_back(MonthlyExpenseEntry{date, amount});
        sortEntries();
    }

    void MonthlyExpense::checkData() {
        sortEntries();
        addMissingEntries();
    }

    void MonthlyExpense::sortEntries() {
        // Sort entries by date
        std::ranges::sort(m_Entries, [](const MonthlyExpenseEntry& a, const MonthlyExpenseEntry& b) {
            return a.date < b.date;
        });
    }

    void MonthlyExpense::addMissingEntries() {
        if (m_Entries.empty()) {
            return;
        }

        // Get the most recent date (last entry after sorting)
        Date lastDate = m_Entries.back().date;
        Date currentDate = getCurrentDate();

        // Start from the next month after the last entry
        Date iterDate = lastDate;
        iterDate.monthIncrement();
        iterDate.day = defaultTransactionDay;

        // Add entries for each missing month
        while (iterDate <= currentDate) {
            // Check if entry already exists for this date
            bool exists = false;
            for (const auto& entry : m_Entries) {
                if (entry.date == iterDate) {
                    exists = true;
                    break;
                }
            }

            if (!exists) {
                m_Entries.emplace_back(MonthlyExpenseEntry{iterDate, defaultTransactionAmount});
            }

            // Move to next month
            iterDate.monthIncrement();
            iterDate.day = defaultTransactionDay;
        }

        // Sort again after adding new entries
        sortEntries();
    }

    void MonthlyExpensesManager::createMonthlyExpense(const std::string &name, const int defaultTransactionDay, const double defaultTransactionAmount) {
        m_MonthlyExpenses.emplace(name, MonthlyExpense(name));
        m_MonthlyExpenses.at(name).defaultTransactionDay = defaultTransactionDay;
        m_MonthlyExpenses.at(name).defaultTransactionAmount = defaultTransactionAmount;
    }

    void MonthlyExpensesManager::destroyMonthlyExpense(const std::string &name) {
        m_MonthlyExpenses.erase(name);
    }

    MonthlyExpense& MonthlyExpensesManager::getMonthlyExpense(const std::string &name) {
        return m_MonthlyExpenses.at(name);
    }

    bool MonthlyExpensesManager::hasMonthlyExpense(const std::string &name) const {
        return m_MonthlyExpenses.contains(name);
    }

    std::unordered_map<std::string, MonthlyExpense>& MonthlyExpensesManager::getMonthlyExpenses() {
        return m_MonthlyExpenses;
    }

    const std::unordered_map<std::string, MonthlyExpense>& MonthlyExpensesManager::getMonthlyExpenses() const {
        return m_MonthlyExpenses;
    }

    void MonthlyExpensesManager::checkData() {
        for (auto &expense: m_MonthlyExpenses | std::views::values) {
            expense.checkData();
        }
    }

}