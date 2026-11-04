#include <FinanceManager/Reports.hpp>

#include <map>
#include <ranges>

namespace FinMgr {

    YearMonth YearMonth::next() const {
        return month >= 12 ? YearMonth{year + 1, 1} : YearMonth{year, month + 1};
    }

    YearMonth YearMonth::previous() const {
        return month <= 1 ? YearMonth{year - 1, 12} : YearMonth{year, month - 1};
    }

    YearMonth YearMonth::plusMonths(const int months) const {
        const int index = year * 12 + (month - 1) + months;
        const int newYear = index >= 0 ? index / 12 : (index - 11) / 12;
        return {newYear, index - newYear * 12 + 1};
    }

    double YearMonth::toUnixTimestamp() const {
        // Days since the epoch of the 15th, from Howard Hinnant's days_from_civil
        const int y = month <= 2 ? year - 1 : year;
        const int era = (y >= 0 ? y : y - 399) / 400;
        const int yearOfEra = y - era * 400;
        const int dayOfYear = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + 15 - 1;
        const int dayOfEra = yearOfEra * 365 + yearOfEra / 4 - yearOfEra / 100 + dayOfYear;
        const double days = static_cast<double>(era) * 146097.0 + dayOfEra - 719468.0;
        return days * 86400.0;
    }

    int monthCount(const YearMonth from, const YearMonth to) {
        const int count = (to.year - from.year) * 12 + (to.month - from.month) + 1;
        return std::max(count, 0);
    }

    namespace {

        template<typename MAP>
        std::vector<std::string> sortedKeys(const MAP& map) {
            std::vector<std::string> keys;
            keys.reserve(map.size());
            for (const auto& key: map | std::views::keys) {
                keys.push_back(key);
            }
            std::ranges::sort(keys);
            return keys;
        }

        template<typename ENTRY>
        double sumInMonth(const std::vector<ENTRY>& entries, const YearMonth month, double (*amount)(const ENTRY&)) {
            double total = 0.0;
            for (const auto& entry: entries) {
                if (month.contains(entry.date)) {
                    total += amount(entry);
                }
            }
            return total;
        }

        double wageAmount(const WageEntry& entry) { return entry.amount + entry.extra; }

        double expenseAmount(const MonthlyExpenseEntry& entry) { return entry.amount; }

        template<typename GROUP, typename ENTRIES>
        void groupByYearNewestFirst(ENTRIES& entries, std::vector<GROUP>& groups) {
            for (auto& entry: std::views::reverse(entries)) {
                if (groups.empty() || groups.back().year != entry.date.year) {
                    groups.emplace_back();
                    groups.back().year = entry.date.year;
                }
                groups.back().entries.push_back(&entry);
            }
        }

    }

    // ------------------------------------------------------------------------------------------------------------------

    ExpenseTable buildExpenseTable(MonthlyExpensesManager& manager, const Date& today) {
        ExpenseTable table;
        auto& expenses = manager.getMonthlyExpenses();
        table.columns = sortedKeys(expenses);
        table.totals.assign(table.columns.size(), 0.0);

        // Index every column by month, so building a row does not scan all entries
        std::vector<std::map<YearMonth, MonthlyExpenseEntry*>> entriesByMonth(table.columns.size());
        std::vector<std::optional<YearMonth>> firstMonths(table.columns.size());
        std::optional<YearMonth> first;
        std::optional<YearMonth> last;
        for (Size col = 0; col < table.columns.size(); ++col) {
            auto& entries = expenses.at(table.columns[col]).getEntries();
            for (auto& entry: entries) {
                const YearMonth month = YearMonth::of(entry.date);
                // Keep the first entry of a month, like the table always did
                entriesByMonth[col].try_emplace(month, &entry);
                if (!firstMonths[col] || month < *firstMonths[col]) {
                    firstMonths[col] = month;
                }
                if (!first || month < *first) {
                    first = month;
                }
                if (!last || month > *last) {
                    last = month;
                }
            }
        }
        if (!first) {
            return table;
        }
        const YearMonth end = std::max(*last, YearMonth::of(today));

        for (YearMonth month = end; month >= *first; month = month.previous()) {
            if (table.years.empty() || table.years.back().year != month.year) {
                table.years.emplace_back();
                table.years.back().year = month.year;
                table.years.back().totals.assign(table.columns.size(), 0.0);
            }
            ExpenseYearGroup& group = table.years.back();

            ExpenseMonthRow row;
            row.month = month;
            row.cells.resize(table.columns.size());
            for (Size col = 0; col < table.columns.size(); ++col) {
                ExpenseCell& cell = row.cells[col];
                cell.started = firstMonths[col].has_value() && month >= *firstMonths[col];
                if (const auto it = entriesByMonth[col].find(month); it != entriesByMonth[col].end()) {
                    cell.entry = it->second;
                    row.total += cell.entry->amount;
                    group.totals[col] += cell.entry->amount;
                    table.totals[col] += cell.entry->amount;
                }
            }
            group.total += row.total;
            table.total += row.total;
            group.months.push_back(std::move(row));
        }
        return table;
    }

    // ------------------------------------------------------------------------------------------------------------------

    WageTable buildWageTable(Wage& wage) {
        WageTable table;
        auto& entries = wage.getEntries();
        groupByYearNewestFirst(entries, table.years);
        for (auto& group: table.years) {
            for (const WageEntry* entry: group.entries) {
                group.totals.add(*entry);
                table.totals.add(*entry);
            }
        }
        return table;
    }

    // ------------------------------------------------------------------------------------------------------------------

    SavingsTable buildSavingsTable(SavingsRecord& record) {
        SavingsTable table;
        table.initialAmount = record.initialAmount;
        table.expected = record.initialAmount;
        table.actual = record.initialAmount;

        // The expected amount accumulates, so walk oldest first and insert the groups in front afterwards
        std::vector<SavingsYearGroup> oldestFirst;
        double expected = record.initialAmount;
        double previousActual = record.initialAmount;
        for (auto& entry: record.getEntries()) {
            if (oldestFirst.empty() || oldestFirst.back().year != entry.date.year) {
                SavingsYearGroup group;
                group.year = entry.date.year;
                group.startActual = previousActual;
                oldestFirst.push_back(std::move(group));
            }
            SavingsYearGroup& group = oldestFirst.back();

            expected += entry.expectedTransactionAmount;
            SavingsRow row;
            row.entry = &entry;
            row.expected = expected;
            row.change = entry.actualState - previousActual;
            previousActual = entry.actualState;

            group.rows.push_back(row);
            group.deposited += entry.expectedTransactionAmount;
            group.endActual = entry.actualState;
            group.endExpected = expected;
            table.deposited += entry.expectedTransactionAmount;
        }
        for (auto& group: std::views::reverse(oldestFirst)) {
            std::ranges::reverse(group.rows);
            table.years.push_back(std::move(group));
        }
        if (!table.years.empty()) {
            table.expected = expected;
            table.actual = previousActual;
        }
        return table;
    }

    // ------------------------------------------------------------------------------------------------------------------

    CashFlow buildCashFlow(const FinanceData& data, const YearMonth from, const YearMonth to) {
        CashFlow flow;
        const int count = monthCount(from, to);
        flow.months.reserve(count);
        for (YearMonth month = from; month <= to; month = month.next()) {
            double income = 0.0;
            for (const auto& wage: data.wageManager.getWages() | std::views::values) {
                income += sumInMonth(wage.getEntries(), month, &wageAmount);
            }
            double expenses = 0.0;
            for (const auto& expense: data.monthlyExpensesManager.getMonthlyExpenses() | std::views::values) {
                expenses += sumInMonth(expense.getEntries(), month, &expenseAmount);
            }
            flow.months.push_back(month);
            flow.times.push_back(month.toUnixTimestamp());
            flow.income.push_back(income);
            flow.expenses.push_back(expenses);
            flow.net.push_back(income - expenses);
        }
        return flow;
    }

    MonthlySeries buildWageSeries(const std::string& name, const Wage& wage, const YearMonth from, const YearMonth to) {
        MonthlySeries series;
        series.name = name;
        for (YearMonth month = from; month <= to; month = month.next()) {
            series.times.push_back(month.toUnixTimestamp());
            series.values.push_back(sumInMonth(wage.getEntries(), month, &wageAmount));
        }
        return series;
    }

    std::vector<MonthlySeries> buildIncomeSeries(const WageManager& manager, const YearMonth from, const YearMonth to) {
        std::vector<MonthlySeries> result;
        for (const auto& name: sortedKeys(manager.getWages())) {
            result.push_back(buildWageSeries(name, manager.getWages().at(name), from, to));
        }
        return result;
    }

    std::vector<MonthlySeries> buildExpenseSeries(const MonthlyExpensesManager& manager, const YearMonth from, const YearMonth to) {
        std::vector<MonthlySeries> result;
        for (const auto& name: sortedKeys(manager.getMonthlyExpenses())) {
            const MonthlyExpense& expense = manager.getMonthlyExpenses().at(name);
            MonthlySeries series;
            series.name = name;
            for (YearMonth month = from; month <= to; month = month.next()) {
                series.times.push_back(month.toUnixTimestamp());
                series.values.push_back(sumInMonth(expense.getEntries(), month, &expenseAmount));
            }
            result.push_back(std::move(series));
        }
        return result;
    }

    SavingsTrend buildSavingsTrend(const SavingsRecord& record, const YearMonth to) {
        SavingsTrend trend;
        const auto& entries = record.getEntries();
        if (entries.empty()) {
            return trend;
        }
        Size next = 0;
        double expected = record.initialAmount;
        double actual = record.initialAmount;
        for (YearMonth month = YearMonth::of(entries.front().date); month <= to; month = month.next()) {
            while (next < entries.size() && entries[next].date <= month.endOfMonth()) {
                expected += entries[next].expectedTransactionAmount;
                actual = entries[next].actualState;
                ++next;
            }
            trend.months.push_back(month);
            trend.times.push_back(month.toUnixTimestamp());
            trend.actual.push_back(actual);
            trend.expected.push_back(expected);
        }
        return trend;
    }

    SavingsTrend buildTotalSavingsTrend(const SavingsManager& manager, const YearMonth to) {
        SavingsTrend total;
        std::optional<YearMonth> first;
        for (const auto& record: manager.getSavingsRecords() | std::views::values) {
            if (!record.getEntries().empty()) {
                const YearMonth month = YearMonth::of(record.getEntries().front().date);
                first = first ? std::min(*first, month) : month;
            }
        }
        if (!first || *first > to) {
            return total;
        }
        for (YearMonth month = *first; month <= to; month = month.next()) {
            total.months.push_back(month);
            total.times.push_back(month.toUnixTimestamp());
        }
        total.actual.assign(total.months.size(), 0.0);
        total.expected.assign(total.months.size(), 0.0);

        for (const auto& record: manager.getSavingsRecords() | std::views::values) {
            const SavingsTrend trend = buildSavingsTrend(record, to);
            if (trend.empty()) {
                continue;
            }
            // The trend of a record starts at its first entry, which is at or after the first month of the total
            const Size offset = monthCount(*first, trend.months.front()) - 1;
            for (Size i = 0; i < trend.months.size(); ++i) {
                total.actual[offset + i] += trend.actual[i];
                total.expected[offset + i] += trend.expected[i];
            }
        }
        return total;
    }

    // ------------------------------------------------------------------------------------------------------------------

    std::optional<YearMonth> firstMonthWithData(const FinanceData& data) {
        std::optional<YearMonth> first;
        const auto consider = [&first](const Date& date) {
            const YearMonth month = YearMonth::of(date);
            first = first ? std::min(*first, month) : month;
        };
        for (const auto& wage: data.wageManager.getWages() | std::views::values) {
            if (!wage.getEntries().empty()) {
                consider(wage.getEntries().front().date);
            }
        }
        for (const auto& expense: data.monthlyExpensesManager.getMonthlyExpenses() | std::views::values) {
            if (!expense.getEntries().empty()) {
                consider(expense.getEntries().front().date);
            }
        }
        for (const auto& record: data.savingsManager.getSavingsRecords() | std::views::values) {
            if (!record.getEntries().empty()) {
                consider(record.getEntries().front().date);
            }
        }
        return first;
    }

    SavingsSnapshot currentSavings(const SavingsManager& manager) {
        SavingsSnapshot snapshot;
        for (const auto& record: manager.getSavingsRecords() | std::views::values) {
            double expected = record.initialAmount;
            for (const auto& entry: record.getEntries()) {
                expected += entry.expectedTransactionAmount;
                snapshot.deposited += entry.expectedTransactionAmount;
            }
            snapshot.expected += expected;
            snapshot.actual += record.getEntries().empty() ? record.initialAmount : record.getEntries().back().actualState;
        }
        return snapshot;
    }

    std::vector<RecurringExpense> currentRecurringExpenses(const MonthlyExpensesManager& manager, const Date& today) {
        std::vector<RecurringExpense> result;
        for (const auto& [name, expense]: manager.getMonthlyExpenses()) {
            RecurringExpense recurring;
            recurring.name = name;
            recurring.day = expense.defaultTransactionDay;
            recurring.amount = expense.defaultTransactionAmount;
            for (const auto& entry: std::views::reverse(expense.getEntries())) {
                if (entry.date <= today) {
                    recurring.amount = entry.amount;
                    break;
                }
            }
            result.push_back(std::move(recurring));
        }
        std::ranges::sort(result, [](const RecurringExpense& a, const RecurringExpense& b) {
            return a.amount != b.amount ? a.amount > b.amount : a.name < b.name;
        });
        return result;
    }

    OLSLinearRegression performOLSLinearRegression(const std::vector<double>& x, const std::vector<double>& y) {
        OLSLinearRegression result;
        if (x.size() != y.size() || x.empty()) {
            return result;
        }
        const double n = static_cast<double>(x.size());
        double sumX = 0.0;
        double sumY = 0.0;
        double sumXX = 0.0;
        double sumXY = 0.0;
        for (Size i = 0; i < x.size(); ++i) {
            sumX += x[i];
            sumY += y[i];
            sumXX += x[i] * x[i];
            sumXY += x[i] * y[i];
        }
        const double denominator = n * sumXX - sumX * sumX;
        if (denominator == 0.0) {
            return result;
        }
        result.slope = (n * sumXY - sumX * sumY) / denominator;
        result.intercept = (sumY - result.slope * sumX) / n;

        double ssTotal = 0.0;
        double ssResidual = 0.0;
        const double meanY = sumY / n;
        for (Size i = 0; i < x.size(); ++i) {
            const double predicted = result.at(x[i]);
            ssTotal += (y[i] - meanY) * (y[i] - meanY);
            ssResidual += (y[i] - predicted) * (y[i] - predicted);
        }
        result.rSquared = ssTotal > 0.0 ? 1.0 - ssResidual / ssTotal : 0.0;
        return result;
    }

}
