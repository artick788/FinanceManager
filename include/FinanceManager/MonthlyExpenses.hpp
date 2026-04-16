#pragma once

#include <FinanceManager/Date.hpp>


namespace FinMgr {

    struct MonthlyExpenseEntry {
        Date date;
        double amount = 0.0;

        VL_GENERATE_JSON_SERIALIZER(MonthlyExpenseEntry, date, amount);
    };

    class MonthlyExpense {
    public:
        int defaultTransactionDay = 0;
        double defaultTransactionAmount = 0.0;

    public:
        MonthlyExpense() = default;

        explicit MonthlyExpense(const std::string& name);

        ~MonthlyExpense() = default;

        void addEntry(const Date& date, double amount);

        std::vector<MonthlyExpenseEntry>& getEntries() { return m_Entries; }

        const std::vector<MonthlyExpenseEntry>& getEntries() const { return m_Entries; }

        VL_GENERATE_JSON_SERIALIZER(MonthlyExpense, m_Name, defaultTransactionDay, defaultTransactionAmount, m_Entries);

    private:
        std::string m_Name;
        std::vector<MonthlyExpenseEntry> m_Entries;
    };

    class MonthlyExpensesManager {
    public:
        MonthlyExpensesManager() = default;

        ~MonthlyExpensesManager() = default;

        void createMonthlyExpense(const std::string& name, int defaultTransactionDay, double defaultTransactionAmount);

        void destroyMonthlyExpense(const std::string& name);

        MonthlyExpense& getMonthlyExpense(const std::string& name);

        bool hasMonthlyExpense(const std::string& name) const;

         std::unordered_map<std::string, MonthlyExpense>& getMonthlyExpenses();

         const std::unordered_map<std::string, MonthlyExpense>& getMonthlyExpenses() const;

        VL_GENERATE_JSON_SERIALIZER(MonthlyExpensesManager, m_MonthlyExpenses);

    private:
        std::unordered_map<std::string, MonthlyExpense> m_MonthlyExpenses;
    };

}