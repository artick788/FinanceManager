#include <gtest/gtest.h>
#include <FinanceManager/MonthlyExpenses.hpp>

using namespace FinMgr;

class TestMonthlyExpense: public ::testing::Test {

};

TEST_F(TestMonthlyExpense, AddEntry) {
    MonthlyExpense expense("Rent");
    
    expense.addEntry(Date(15, 3, 2025), 1000.0);
    expense.addEntry(Date(15, 1, 2025), 1000.0);
    expense.addEntry(Date(15, 2, 2025), 1000.0);
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 3);
    
    // Verify entries are sorted by date
    EXPECT_EQ(entries[0].date, Date(15, 1, 2025));
    EXPECT_EQ(entries[1].date, Date(15, 2, 2025));
    EXPECT_EQ(entries[2].date, Date(15, 3, 2025));
}

TEST_F(TestMonthlyExpense, AddEntryOutOfOrder) {
    MonthlyExpense expense("Utilities");
    
    expense.addEntry(Date(10, 12, 2024), 150.0);
    expense.addEntry(Date(10, 1, 2025), 160.0);
    expense.addEntry(Date(10, 6, 2024), 140.0);
    expense.addEntry(Date(10, 3, 2025), 155.0);
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 4);
    
    // Verify entries are sorted correctly
    EXPECT_EQ(entries[0].date, Date(10, 6, 2024));
    EXPECT_EQ(entries[1].date, Date(10, 12, 2024));
    EXPECT_EQ(entries[2].date, Date(10, 1, 2025));
    EXPECT_EQ(entries[3].date, Date(10, 3, 2025));
}

TEST_F(TestMonthlyExpense, AddEntryWithAmounts) {
    MonthlyExpense expense("Subscription");
    
    expense.addEntry(Date(1, 1, 2025), 9.99);
    expense.addEntry(Date(1, 2, 2025), 10.99);
    expense.addEntry(Date(1, 3, 2025), 10.99);
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 3);
    
    EXPECT_EQ(entries[0].amount, 9.99);
    EXPECT_EQ(entries[1].amount, 10.99);
    EXPECT_EQ(entries[2].amount, 10.99);
}

TEST_F(TestMonthlyExpense, SortEntriesManuallyAdded) {
    MonthlyExpense expense("Rent");
    
    // Manually add unsorted entries
    auto& entries = expense.getEntries();
    entries.push_back({Date(15, 5, 2025), 1000.0});
    entries.push_back({Date(15, 1, 2025), 1000.0});
    entries.push_back({Date(15, 3, 2025), 1000.0});
    
    // Now sort them
    expense.sortEntries();
    
    ASSERT_EQ(entries.size(), 3);
    EXPECT_EQ(entries[0].date, Date(15, 1, 2025));
    EXPECT_EQ(entries[1].date, Date(15, 3, 2025));
    EXPECT_EQ(entries[2].date, Date(15, 5, 2025));
}

TEST_F(TestMonthlyExpense, CheckDataEmpty) {
    MonthlyExpense expense("Rent");
    expense.defaultTransactionDay = 1;
    expense.defaultTransactionAmount = 1200.0;
    
    // CheckData on empty list should not crash
    expense.checkData();
    
    auto& entries = expense.getEntries();
    EXPECT_EQ(entries.size(), 0);
}

TEST_F(TestMonthlyExpense, CheckDataSortsEntries) {
    MonthlyExpense expense("Rent");
    
    // Manually add unsorted entries
    auto& entries = expense.getEntries();
    entries.push_back({Date(15, 5, 2025), 1000.0});
    entries.push_back({Date(15, 1, 2025), 1000.0});
    entries.push_back({Date(15, 3, 2025), 1000.0});
    
    // CheckData should sort them
    expense.checkData();

    EXPECT_EQ(entries[0].date, Date(15, 1, 2025));
    EXPECT_EQ(entries[1].date, Date(15, 3, 2025));
    EXPECT_EQ(entries[2].date, Date(15, 5, 2025));
}

TEST_F(TestMonthlyExpense, CheckDataAddsMissingEntriesSingleMonth) {
    MonthlyExpense expense("Rent");
    expense.defaultTransactionDay = 1;
    expense.defaultTransactionAmount = 1200.0;
    
    // Add an entry from two months ago
    expense.addEntry(Date(1, 2, 2026), 1200.0);
    
    // CheckData should add missing entry for March
    expense.checkData();
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 3);  // Feb, Mar, Apr
    EXPECT_EQ(entries[0].date, Date(1, 2, 2026));
    EXPECT_EQ(entries[0].amount, 1200.0);
    EXPECT_EQ(entries[1].date, Date(1, 3, 2026));
    EXPECT_EQ(entries[1].amount, 1200.0);
    EXPECT_EQ(entries[2].date, Date(1, 4, 2026));
    EXPECT_EQ(entries[2].amount, 1200.0);
}

TEST_F(TestMonthlyExpense, CheckDataAddsMissingEntriesMultipleMonths) {
    MonthlyExpense expense("Rent");
    expense.defaultTransactionDay = 15;
    expense.defaultTransactionAmount = 1500.0;
    
    // Add an entry from January 2026
    expense.addEntry(Date(15, 1, 2026), 1500.0);
    
    // CheckData should add missing entries up to current date (April 2026)
    expense.checkData();
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 4);  // Jan, Feb, Mar, Apr
    EXPECT_EQ(entries[0].date, Date(15, 1, 2026));
    EXPECT_EQ(entries[1].date, Date(15, 2, 2026));
    EXPECT_EQ(entries[2].date, Date(15, 3, 2026));
    EXPECT_EQ(entries[3].date, Date(15, 4, 2026));
    
    // Check all amounts are correct
    for (const auto& entry : entries) {
        EXPECT_EQ(entry.amount, 1500.0);
    }
}

TEST_F(TestMonthlyExpense, CheckDataAddsMissingEntriesAcrossYears) {
    MonthlyExpense expense("Subscription");
    expense.defaultTransactionDay = 5;
    expense.defaultTransactionAmount = 25.0;
    
    // Add an entry from November 2025
    expense.addEntry(Date(5, 11, 2025), 25.0);
    
    // CheckData should add missing entries up to current date (April 2026)
    expense.checkData();
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 6);  // Nov, Dec, Jan, Feb, Mar, Apr
    EXPECT_EQ(entries[0].date, Date(5, 11, 2025));
    EXPECT_EQ(entries[1].date, Date(5, 12, 2025));
    EXPECT_EQ(entries[2].date, Date(5, 1, 2026));
    EXPECT_EQ(entries[3].date, Date(5, 2, 2026));
    EXPECT_EQ(entries[4].date, Date(5, 3, 2026));
    EXPECT_EQ(entries[5].date, Date(5, 4, 2026));
}

TEST_F(TestMonthlyExpense, CheckDataDoesNotAddDuplicates) {
    MonthlyExpense expense("Rent");
    expense.defaultTransactionDay = 1;
    expense.defaultTransactionAmount = 1200.0;
    
    // Add entries that already exist
    expense.addEntry(Date(1, 1, 2026), 1200.0);
    expense.addEntry(Date(1, 2, 2026), 1200.0);
    expense.addEntry(Date(1, 3, 2026), 1200.0);
    
    // CheckData should not add duplicates
    expense.checkData();
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 4);  // Jan, Feb, Mar, Apr (no duplicates)
}

TEST_F(TestMonthlyExpense, CheckDataWithDifferentAmounts) {
    MonthlyExpense expense("Utilities");
    expense.defaultTransactionDay = 15;
    expense.defaultTransactionAmount = 80.0;
    
    // Add entries with different amounts than default
    expense.addEntry(Date(15, 1, 2026), 90.0);
    expense.addEntry(Date(15, 2, 2026), 95.0);
    
    // CheckData should add missing entries with default amount
    expense.checkData();
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 4);  // Jan, Feb, Mar, Apr
    EXPECT_EQ(entries[0].amount, 90.0);   // Original
    EXPECT_EQ(entries[1].amount, 95.0);   // Original
    EXPECT_EQ(entries[2].amount, 80.0);   // Default
    EXPECT_EQ(entries[3].amount, 80.0);   // Default
}

class TestMonthlyExpensesManager: public ::testing::Test {

};

TEST_F(TestMonthlyExpensesManager, CreateAndHasMonthlyExpense) {
    MonthlyExpensesManager manager;
    
    EXPECT_FALSE(manager.hasMonthlyExpense("Rent"));
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    
    EXPECT_TRUE(manager.hasMonthlyExpense("Rent"));
}

TEST_F(TestMonthlyExpensesManager, CreateMultipleMonthlyExpenses) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    manager.createMonthlyExpense("Utilities", 10, 150.0);
    manager.createMonthlyExpense("Internet", 5, 60.0);
    
    EXPECT_TRUE(manager.hasMonthlyExpense("Rent"));
    EXPECT_TRUE(manager.hasMonthlyExpense("Utilities"));
    EXPECT_TRUE(manager.hasMonthlyExpense("Internet"));
    EXPECT_FALSE(manager.hasMonthlyExpense("NonExistent"));
}

TEST_F(TestMonthlyExpensesManager, DestroyMonthlyExpense) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    manager.createMonthlyExpense("Utilities", 10, 150.0);
    
    EXPECT_TRUE(manager.hasMonthlyExpense("Rent"));
    EXPECT_TRUE(manager.hasMonthlyExpense("Utilities"));
    
    manager.destroyMonthlyExpense("Rent");
    
    EXPECT_FALSE(manager.hasMonthlyExpense("Rent"));
    EXPECT_TRUE(manager.hasMonthlyExpense("Utilities"));
}

TEST_F(TestMonthlyExpensesManager, GetMonthlyExpense) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    MonthlyExpense& expense = manager.getMonthlyExpense("Rent");
    
    expense.addEntry(Date(1, 1, 2026), 1200.0);
    
    auto& entries = expense.getEntries();
    ASSERT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0].amount, 1200.0);
}

TEST_F(TestMonthlyExpensesManager, CreateWithDefaultValues) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 15, 1500.0);
    MonthlyExpense& expense = manager.getMonthlyExpense("Rent");
    
    EXPECT_EQ(expense.defaultTransactionDay, 15);
    EXPECT_EQ(expense.defaultTransactionAmount, 1500.0);
}

TEST_F(TestMonthlyExpensesManager, CheckDataCallsAllExpenses) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    manager.createMonthlyExpense("Utilities", 10, 150.0);
    
    // Add old entries to both
    MonthlyExpense& rent = manager.getMonthlyExpense("Rent");
    rent.addEntry(Date(1, 1, 2026), 1200.0);
    
    MonthlyExpense& utilities = manager.getMonthlyExpense("Utilities");
    utilities.addEntry(Date(10, 1, 2026), 150.0);
    
    // CheckData should process all expenses
    manager.checkData();
    
    auto& rentEntries = rent.getEntries();
    auto& utilEntries = utilities.getEntries();
    
    // Both should have entries added up to current date
    EXPECT_GT(rentEntries.size(), 1);
    EXPECT_GT(utilEntries.size(), 1);
    
    // Verify last entries are for April 2026
    EXPECT_EQ(rentEntries.back().date, Date(1, 4, 2026));
    EXPECT_EQ(utilEntries.back().date, Date(10, 4, 2026));
}

TEST_F(TestMonthlyExpensesManager, CheckDataWithEmptyExpenses) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    manager.createMonthlyExpense("Utilities", 10, 150.0);
    
    // Don't add any entries, just call checkData
    manager.checkData();
    
    // Should not crash and expenses should remain empty
    MonthlyExpense& rent = manager.getMonthlyExpense("Rent");
    MonthlyExpense& utilities = manager.getMonthlyExpense("Utilities");
    
    EXPECT_EQ(rent.getEntries().size(), 0);
    EXPECT_EQ(utilities.getEntries().size(), 0);
}

TEST_F(TestMonthlyExpensesManager, CheckDataSortsUnsortedData) {
    MonthlyExpensesManager manager;
    
    manager.createMonthlyExpense("Rent", 1, 1200.0);
    
    MonthlyExpense& rent = manager.getMonthlyExpense("Rent");
    
    // Manually add unsorted entries
    auto& entries = rent.getEntries();
    entries.push_back({Date(1, 3, 2026), 1200.0});
    entries.push_back({Date(1, 1, 2026), 1200.0});
    entries.push_back({Date(1, 2, 2026), 1200.0});
    
    // CheckData should sort and add missing entries
    manager.checkData();
    
    ASSERT_GE(entries.size(), 3);
    EXPECT_EQ(entries[0].date, Date(1, 1, 2026));
    EXPECT_EQ(entries[1].date, Date(1, 2, 2026));
    EXPECT_EQ(entries[2].date, Date(1, 3, 2026));
}
