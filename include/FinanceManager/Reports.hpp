#pragma once

#include <FinanceManager/FinanceData.hpp>

/*
 * Reports turn the raw entries into the rows and series the UI draws. They are rebuilt every frame, so they are cheap
 * and never cache anything. Tables keep pointers to the entries so cells can be edited in place, a table is therefore
 * only valid until entries are added or removed.
 *
 * Every table is ordered newest first, since the recent data is what is looked at the most.
 */

namespace FinMgr {

    struct YearMonth {
        int year = 0;
        int month = 0;

        static YearMonth of(const Date& date) { return {date.year, date.month}; }

        YearMonth next() const;

        YearMonth previous() const;

        YearMonth plusMonths(int months) const;

        Date firstDay() const { return {1, month, year}; }

        /**
         * @brief Day 31 for every month, dates only compare by their integer value so this is the end of every month
         */
        Date endOfMonth() const { return {31, month, year}; }

        bool contains(const Date& date) const { return date.year == year && date.month == month; }

        /**
         * @return The unix timestamp (UTC) of the middle of the month, used as x value in plots. The middle keeps every
         *        point inside its month whatever the timezone of the plot labels is.
         */
        double toUnixTimestamp() const;

        auto operator<=>(const YearMonth&) const = default;
    };

    /**
     * @return The number of months in [from, to], 0 if to is before from
     */
    int monthCount(YearMonth from, YearMonth to);

    // ------------------------------------------------------------------------------------------------------------------
    // Monthly expenses: one row per month, one column per expense

    struct ExpenseCell {
        MonthlyExpenseEntry* entry = nullptr; // The entry of this month, nullptr if there is none
        bool started = false;                 // True if the expense already existed in this month
    };

    struct ExpenseMonthRow {
        YearMonth month;
        std::vector<ExpenseCell> cells; // One per column
        double total = 0.0;
    };

    struct ExpenseYearGroup {
        int year = 0;
        std::vector<ExpenseMonthRow> months; // Newest first
        std::vector<double> totals;          // One per column
        double total = 0.0;
    };

    struct ExpenseTable {
        std::vector<std::string> columns;     // Expense names, sorted alphabetically
        std::vector<ExpenseYearGroup> years;  // Newest first
        std::vector<double> totals;           // One per column
        double total = 0.0;

        bool empty() const { return years.empty(); }
    };

    /**
     * @brief Builds the table from the month of the oldest entry up to the current month (or the month of the newest
     *        entry if that is later).
     */
    ExpenseTable buildExpenseTable(MonthlyExpensesManager& manager, const Date& today);

    // ------------------------------------------------------------------------------------------------------------------
    // Income: one row per paycheck

    struct WageTotals {
        double check = 0.0;
        double extra = 0.0;

        double total() const { return check + extra; }

        void add(const WageEntry& entry) { check += entry.amount; extra += entry.extra; }
    };

    struct WageYearGroup {
        int year = 0;
        std::vector<WageEntry*> entries; // Newest first
        WageTotals totals;
    };

    struct WageTable {
        std::vector<WageYearGroup> years; // Newest first
        WageTotals totals;

        bool empty() const { return years.empty(); }
    };

    WageTable buildWageTable(Wage& wage);

    // ------------------------------------------------------------------------------------------------------------------
    // Savings: one row per entry, with the expected amount accumulated from the initial amount

    struct SavingsRow {
        SavingEntry* entry = nullptr;
        double expected = 0.0; // Initial amount + all expected transactions up to and including this entry
        double change = 0.0;   // Actual state minus the actual state of the previous entry (or the initial amount)

        double variance() const { return entry->actualState - expected; }
    };

    struct SavingsYearGroup {
        int year = 0;
        std::vector<SavingsRow> rows; // Newest first
        double deposited = 0.0;       // Sum of the expected transactions of this year
        double startActual = 0.0;     // Actual state at the end of the previous year (or the initial amount)
        double endActual = 0.0;       // Actual state of the last entry of this year
        double endExpected = 0.0;     // Expected amount after the last entry of this year

        double growth() const { return endActual - startActual; }

        /**
         * @brief The growth that does not come from deposits, e.g. interest or market gains
         */
        double gain() const { return growth() - deposited; }
    };

    struct SavingsTable {
        double initialAmount = 0.0;
        std::vector<SavingsYearGroup> years; // Newest first
        double deposited = 0.0;
        double expected = 0.0; // Expected amount after the newest entry
        double actual = 0.0;   // Actual state of the newest entry, the initial amount if there are no entries

        double variance() const { return actual - expected; }

        bool empty() const { return years.empty(); }
    };

    SavingsTable buildSavingsTable(SavingsRecord& record);

    // ------------------------------------------------------------------------------------------------------------------
    // Monthly series, oldest first, used for plots and the dashboard

    struct CashFlow {
        std::vector<YearMonth> months;
        std::vector<double> times; // Unix timestamps of the months, for plotting
        std::vector<double> income;
        std::vector<double> expenses;
        std::vector<double> net;

        bool empty() const { return months.empty(); }
    };

    /**
     * @brief Income and expenses for every month in [from, to]
     */
    CashFlow buildCashFlow(const FinanceData& data, YearMonth from, YearMonth to);

    struct MonthlySeries {
        std::string name;
        std::vector<double> times;
        std::vector<double> values;
    };

    /**
     * @brief The total income (paycheck + extra) of every month in [from, to] of a single wage
     */
    MonthlySeries buildWageSeries(const std::string& name, const Wage& wage, YearMonth from, YearMonth to);

    /**
     * @brief The total income of every month in [from, to], per wage (sorted by name)
     */
    std::vector<MonthlySeries> buildIncomeSeries(const WageManager& manager, YearMonth from, YearMonth to);

    /**
     * @brief The amount of every expense of every month in [from, to], per expense (sorted by name)
     */
    std::vector<MonthlySeries> buildExpenseSeries(const MonthlyExpensesManager& manager, YearMonth from, YearMonth to);

    struct SavingsTrend {
        std::vector<YearMonth> months;
        std::vector<double> times;
        std::vector<double> actual;   // The last known actual state at the end of every month
        std::vector<double> expected; // Initial amount + expected transactions up to the end of every month

        bool empty() const { return months.empty(); }
    };

    /**
     * @brief Trend of a single record, from the month of its first entry up to the given month. Months without an
     *        entry carry the last known actual state forward.
     */
    SavingsTrend buildSavingsTrend(const SavingsRecord& record, YearMonth to);

    /**
     * @brief Sum of the trends of all records. A record only contributes from the month of its first entry on.
     */
    SavingsTrend buildTotalSavingsTrend(const SavingsManager& manager, YearMonth to);

    // ------------------------------------------------------------------------------------------------------------------
    // Ranges and statistics

    /**
     * @return The month of the oldest entry in the data, std::nullopt if there are no entries at all
     */
    std::optional<YearMonth> firstMonthWithData(const FinanceData& data);

    struct SavingsSnapshot {
        double actual = 0.0;    // Sum of the newest actual state of every record
        double expected = 0.0;  // Sum of the expected amount of every record
        double deposited = 0.0; // Sum of all expected transactions

        double variance() const { return actual - expected; }
    };

    SavingsSnapshot currentSavings(const SavingsManager& manager);

    struct RecurringExpense {
        std::string name;
        double amount = 0.0; // Amount of the newest entry up to today, the default amount if there is none
        int day = 0;         // Default transaction day
    };

    /**
     * @return The current amount of every monthly expense, sorted from most to least expensive
     */
    std::vector<RecurringExpense> currentRecurringExpenses(const MonthlyExpensesManager& manager, const Date& today);

    /**
     * @brief Results for the Ordinary Least Squares (OLS) linear regression.
     */
    struct OLSLinearRegression {
        double slope = 0.0;
        double intercept = 0.0;
        double rSquared = 0.0;

        double at(const double x) const { return slope * x + intercept; }
    };

    OLSLinearRegression performOLSLinearRegression(const std::vector<double>& x, const std::vector<double>& y);

    constexpr double SECONDS_PER_MONTH = 30.0 * 24.0 * 3600.0;

}
