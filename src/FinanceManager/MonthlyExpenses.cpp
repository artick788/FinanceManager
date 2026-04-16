#include <FinanceManager/MonthlyExpenses.hpp>

namespace FinMgr {

    MonthlyExpense::MonthlyExpense(const std::string &name):
    m_Name(name){

    }

    void MonthlyExpense::addEntry(const Date &date, const double amount) {
        m_Entries.emplace_back(MonthlyExpenseEntry{date, amount});
        // Sort entries by date after adding a new one
        std::ranges::sort(m_Entries, [](const MonthlyExpenseEntry& a, const MonthlyExpenseEntry& b) {
            return a.date < b.date;
        });
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

}