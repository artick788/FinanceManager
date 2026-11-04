#include <gtest/gtest.h>
#include <FinanceManager/Reports.hpp>

using namespace FinMgr;

TEST(TestYearMonth, NextAndPrevious) {
    EXPECT_EQ((YearMonth{2025, 12}.next()), (YearMonth{2026, 1}));
    EXPECT_EQ((YearMonth{2026, 1}.previous()), (YearMonth{2025, 12}));
    EXPECT_EQ((YearMonth{2026, 3}.plusMonths(-14)), (YearMonth{2025, 1}));
    EXPECT_EQ((YearMonth{2026, 3}.plusMonths(10)), (YearMonth{2027, 1}));
    EXPECT_EQ(monthCount({2025, 11}, {2026, 2}), 4);
    EXPECT_EQ(monthCount({2026, 2}, {2025, 11}), 0);
    EXPECT_DOUBLE_EQ((YearMonth{1970, 1}.toUnixTimestamp()), 14.0 * 86400.0);
    EXPECT_DOUBLE_EQ((YearMonth{2026, 3}.toUnixTimestamp()), 1773532800.0); // 2026-03-15T00:00:00Z
}

TEST(TestExpenseTable, NewestFirstGroupedByYear) {
    MonthlyExpensesManager manager;
    manager.createMonthlyExpense("Rent", 1, 1000.0);
    manager.createMonthlyExpense("Internet", 5, 40.0);
    manager.getMonthlyExpense("Rent").addEntry(Date(1, 11, 2025), 1000.0);
    manager.getMonthlyExpense("Rent").addEntry(Date(1, 12, 2025), 1000.0);
    manager.getMonthlyExpense("Rent").addEntry(Date(1, 1, 2026), 1100.0);
    manager.getMonthlyExpense("Internet").addEntry(Date(5, 1, 2026), 40.0);

    const ExpenseTable table = buildExpenseTable(manager, Date(10, 2, 2026));

    ASSERT_EQ(table.columns, (std::vector<std::string>{"Internet", "Rent"}));
    ASSERT_EQ(table.years.size(), 2u);

    // 2026 comes first, with February (no entries yet) on top
    const ExpenseYearGroup& year2026 = table.years[0];
    EXPECT_EQ(year2026.year, 2026);
    ASSERT_EQ(year2026.months.size(), 2u);
    EXPECT_EQ(year2026.months[0].month, (YearMonth{2026, 2}));
    EXPECT_EQ(year2026.months[0].cells[1].entry, nullptr);
    EXPECT_TRUE(year2026.months[0].cells[1].started); // Shows N/A
    EXPECT_EQ(year2026.months[1].month, (YearMonth{2026, 1}));
    EXPECT_DOUBLE_EQ(year2026.months[1].total, 1140.0);
    EXPECT_DOUBLE_EQ(year2026.totals[0], 40.0);
    EXPECT_DOUBLE_EQ(year2026.totals[1], 1100.0);

    const ExpenseYearGroup& year2025 = table.years[1];
    EXPECT_EQ(year2025.year, 2025);
    ASSERT_EQ(year2025.months.size(), 2u);
    EXPECT_EQ(year2025.months[0].month, (YearMonth{2025, 12}));
    EXPECT_EQ(year2025.months[1].month, (YearMonth{2025, 11}));
    EXPECT_FALSE(year2025.months[1].cells[0].started); // Internet did not exist yet
    EXPECT_DOUBLE_EQ(year2025.total, 2000.0);

    EXPECT_DOUBLE_EQ(table.total, 3140.0);
}

TEST(TestExpenseTable, CellsEditEntriesInPlace) {
    MonthlyExpensesManager manager;
    manager.createMonthlyExpense("Rent", 1, 1000.0);
    manager.getMonthlyExpense("Rent").addEntry(Date(1, 1, 2026), 1000.0);

    const ExpenseTable table = buildExpenseTable(manager, Date(1, 1, 2026));
    table.years[0].months[0].cells[0].entry->amount = 1234.0;

    EXPECT_DOUBLE_EQ(manager.getMonthlyExpense("Rent").getEntries()[0].amount, 1234.0);
}

TEST(TestExpenseTable, Empty) {
    MonthlyExpensesManager manager;
    manager.createMonthlyExpense("Rent", 1, 1000.0);
    EXPECT_TRUE(buildExpenseTable(manager, Date(1, 1, 2026)).empty());
}

TEST(TestWageTable, NewestFirstGroupedByYear) {
    Wage wage("Job");
    wage.addEntry({Date(25, 12, 2025), 3000.0, 500.0});
    wage.addEntry({Date(25, 1, 2026), 3100.0, 0.0});
    wage.addEntry({Date(25, 2, 2026), 3100.0, 50.0});

    const WageTable table = buildWageTable(wage);
    ASSERT_EQ(table.years.size(), 2u);
    EXPECT_EQ(table.years[0].year, 2026);
    ASSERT_EQ(table.years[0].entries.size(), 2u);
    EXPECT_EQ(table.years[0].entries[0]->date, Date(25, 2, 2026));
    EXPECT_DOUBLE_EQ(table.years[0].totals.total(), 6250.0);
    EXPECT_DOUBLE_EQ(table.years[1].totals.extra, 500.0);
    EXPECT_DOUBLE_EQ(table.totals.total(), 9750.0);
}

TEST(TestSavingsTable, AccumulatesExpectedAndYearGrowth) {
    SavingsRecord record("Account", 1000.0);
    record.addEntry(Date(1, 11, 2025), 100.0, 1110.0);
    record.addEntry(Date(1, 12, 2025), 100.0, 1220.0);
    record.addEntry(Date(1, 1, 2026), 100.0, 1300.0);

    const SavingsTable table = buildSavingsTable(record);
    ASSERT_EQ(table.years.size(), 2u);

    const SavingsYearGroup& year2026 = table.years[0];
    EXPECT_EQ(year2026.year, 2026);
    ASSERT_EQ(year2026.rows.size(), 1u);
    EXPECT_DOUBLE_EQ(year2026.rows[0].expected, 1300.0);
    EXPECT_DOUBLE_EQ(year2026.rows[0].change, 80.0);
    EXPECT_DOUBLE_EQ(year2026.startActual, 1220.0);

    const SavingsYearGroup& year2025 = table.years[1];
    ASSERT_EQ(year2025.rows.size(), 2u);
    EXPECT_EQ(year2025.rows[0].entry->date, Date(1, 12, 2025)); // Newest first
    EXPECT_DOUBLE_EQ(year2025.rows[0].expected, 1200.0);
    EXPECT_DOUBLE_EQ(year2025.rows[0].variance(), 20.0);
    EXPECT_DOUBLE_EQ(year2025.deposited, 200.0);
    EXPECT_DOUBLE_EQ(year2025.growth(), 220.0);
    EXPECT_DOUBLE_EQ(year2025.gain(), 20.0);

    EXPECT_DOUBLE_EQ(table.expected, 1300.0);
    EXPECT_DOUBLE_EQ(table.actual, 1300.0);
    EXPECT_DOUBLE_EQ(table.deposited, 300.0);
}

TEST(TestSavingsTrend, CarriesActualForwardAndSumsRecords) {
    SavingsManager manager;
    manager.createSavingsRecord("A", 100.0);
    manager.createSavingsRecord("B", 0.0);
    manager.getSavingsRecord("A").addEntry(Date(15, 1, 2026), 10.0, 120.0);
    manager.getSavingsRecord("A").addEntry(Date(15, 3, 2026), 10.0, 140.0);
    manager.getSavingsRecord("B").addEntry(Date(1, 2, 2026), 50.0, 50.0);

    const SavingsTrend trend = buildTotalSavingsTrend(manager, {2026, 3});
    ASSERT_EQ(trend.months.size(), 3u);
    EXPECT_DOUBLE_EQ(trend.actual[0], 120.0);
    EXPECT_DOUBLE_EQ(trend.actual[1], 170.0); // A carried forward
    EXPECT_DOUBLE_EQ(trend.actual[2], 190.0);
    EXPECT_DOUBLE_EQ(trend.expected[2], 170.0);
}

TEST(TestCashFlow, IncomeMinusExpenses) {
    FinanceData data;
    data.wageManager.createWage("Job");
    data.wageManager.getWage("Job").addEntry({Date(25, 1, 2026), 3000.0, 200.0});
    data.monthlyExpensesManager.createMonthlyExpense("Rent", 1, 1000.0);
    data.monthlyExpensesManager.getMonthlyExpense("Rent").addEntry(Date(1, 1, 2026), 1000.0);
    data.monthlyExpensesManager.getMonthlyExpense("Rent").addEntry(Date(1, 2, 2026), 1000.0);

    const CashFlow flow = buildCashFlow(data, {2026, 1}, {2026, 2});
    ASSERT_EQ(flow.months.size(), 2u);
    EXPECT_DOUBLE_EQ(flow.income[0], 3200.0);
    EXPECT_DOUBLE_EQ(flow.net[0], 2200.0);
    EXPECT_DOUBLE_EQ(flow.net[1], -1000.0);
}

TEST(TestRecurringExpenses, UsesNewestEntryUpToToday) {
    MonthlyExpensesManager manager;
    manager.createMonthlyExpense("Rent", 1, 900.0);
    manager.createMonthlyExpense("Gym", 3, 30.0);
    manager.getMonthlyExpense("Rent").addEntry(Date(1, 1, 2026), 1000.0);
    manager.getMonthlyExpense("Rent").addEntry(Date(1, 3, 2026), 2000.0); // In the future

    const auto expenses = currentRecurringExpenses(manager, Date(10, 2, 2026));
    ASSERT_EQ(expenses.size(), 2u);
    EXPECT_EQ(expenses[0].name, "Rent");
    EXPECT_DOUBLE_EQ(expenses[0].amount, 1000.0);
    EXPECT_DOUBLE_EQ(expenses[1].amount, 30.0); // Default amount, no entries
}

TEST(TestRemoveEntry, OnlyRemovesOwnEntries) {
    Wage wage("Job");
    wage.addEntry({Date(25, 1, 2026), 3000.0, 0.0});
    wage.addEntry({Date(25, 2, 2026), 3000.0, 0.0});
    const WageEntry other{};

    EXPECT_FALSE(wage.removeEntry(&other));
    EXPECT_TRUE(wage.removeEntry(&wage.getEntries()[0]));
    ASSERT_EQ(wage.getEntries().size(), 1u);
    EXPECT_EQ(wage.getEntries()[0].date, Date(25, 2, 2026));
}

// ---------------------------------------------------------------------------------------------------------------------
// The on-disk format must stay readable

static const char* LEGACY_SETTINGS = R"({
    "AppSettings": {
        "Store": {
            "wageManager": {"m_Wages": {"Job": {"m_Name": "Job", "m_Entries": [{"date": 20260125, "amount": 3000.0, "extra": 100.0}]}}},
            "savingsManager": {"m_SavingsRecords": {"Account": {"m_Name": "Account", "initialAmount": 500.0, "defaultTransactionAmount": 50.0,
                "m_Entries": [{"date": 20260101, "expectedTransactionAmount": 50.0, "actualState": 560.0}]}}},
            "monthlyExpensesManager": {"m_MonthlyExpenses": {"Rent": {"m_Name": "Rent", "defaultTransactionDay": 1, "defaultTransactionAmount": 1000.0,
                "m_Entries": [{"date": 20260101, "amount": 1000.0}]}}}
        }
    },
    "GitInfo": {"saveToGit": false, "gitRepoPath": "", "sshString": ""},
    "WindowSettings": {"width": 1280, "height": 720}
})";

static std::optional<nlohmann::json> opt(const nlohmann::json& json) {
    return std::optional<nlohmann::json>(std::in_place, json);
}

TEST(TestFinanceDataLoad, MigratesLegacyLayout) {
    const nlohmann::json settings = nlohmann::json::parse(LEGACY_SETTINGS);
    FinanceDataLoadResult result = loadFinanceData(std::nullopt, opt(settings.at("AppSettings")));

    EXPECT_EQ(result.source, FinanceDataSource::Legacy);
    EXPECT_TRUE(result.error.empty());
    ASSERT_TRUE(result.data.wageManager.hasWage("Job"));
    EXPECT_DOUBLE_EQ(result.data.wageManager.getWage("Job").getEntries()[0].extra, 100.0);
    EXPECT_DOUBLE_EQ(result.data.savingsManager.getSavingsRecord("Account").initialAmount, 500.0);
    EXPECT_TRUE(result.data.monthlyExpensesManager.hasMonthlyExpense("Rent"));
}

TEST(TestFinanceDataLoad, RoundTripKeepsTheSameJson) {
    const nlohmann::json settings = nlohmann::json::parse(LEGACY_SETTINGS);
    const nlohmann::json& store = settings.at("AppSettings").at("Store");

    FinanceData data;
    data.fromJson(store);
    nlohmann::json saved = data.toJson();

    // Saving must produce exactly the same layout, so older versions of the app can still read it
    EXPECT_EQ(saved["wageManager"], store["wageManager"]);
    EXPECT_EQ(saved["savingsManager"], store["savingsManager"]);
    EXPECT_EQ(saved["monthlyExpensesManager"], store["monthlyExpensesManager"]);
}

TEST(TestFinanceDataLoad, PrefersNewStoreKey) {
    const nlohmann::json settings = nlohmann::json::parse(LEGACY_SETTINGS);
    nlohmann::json store = settings.at("AppSettings").at("Store");
    store["wageManager"]["m_Wages"].erase("Job");

    const FinanceDataLoadResult result = loadFinanceData(opt(store), opt(settings.at("AppSettings")));
    EXPECT_EQ(result.source, FinanceDataSource::Store);
    EXPECT_FALSE(result.data.wageManager.hasWage("Job"));
}

TEST(TestFinanceDataLoad, ReportsCorruptData) {
    nlohmann::json store = nlohmann::json::parse(LEGACY_SETTINGS).at("AppSettings").at("Store");
    store["wageManager"]["m_Wages"]["Job"]["m_Entries"][0].erase("amount");

    const FinanceDataLoadResult result = loadFinanceData(opt(store), std::nullopt);
    EXPECT_EQ(result.source, FinanceDataSource::Store);
    EXPECT_FALSE(result.error.empty());
}

TEST(TestFinanceDataLoad, FreshInstall) {
    const FinanceDataLoadResult result = loadFinanceData(std::nullopt, std::nullopt);
    EXPECT_EQ(result.source, FinanceDataSource::None);
    EXPECT_TRUE(result.error.empty());
}
