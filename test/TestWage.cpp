#include <gtest/gtest.h>
#include <FinanceManager/Wage.hpp>

using namespace FinMgr;

class TestWage: public ::testing::Test {

};

TEST_F(TestWage, AddEntry) {
    Wage wage("TestWage");
    
    WageEntry e1{Date(15, 3, 2025), 1000.0, 50.0};
    WageEntry e2{Date(15, 1, 2025), 1000.0, 50.0};
    WageEntry e3{Date(15, 2, 2025), 1000.0, 50.0};
    
    wage.addEntry(e1);
    wage.addEntry(e2);
    wage.addEntry(e3);
    
    auto& entries = wage.getEntries();
    ASSERT_EQ(entries.size(), 3);
    
    // Verify entries are sorted by date
    EXPECT_EQ(entries[0].date, Date(15, 1, 2025));
    EXPECT_EQ(entries[1].date, Date(15, 2, 2025));
    EXPECT_EQ(entries[2].date, Date(15, 3, 2025));
}

TEST_F(TestWage, AddEntryOutOfOrder) {
    Wage wage("TestWage");
    
    wage.addEntry({Date(15, 12, 2024), 1000.0, 0.0});
    wage.addEntry({Date(15, 1, 2025), 1000.0, 0.0});
    wage.addEntry({Date(15, 6, 2024), 1000.0, 0.0});
    wage.addEntry({Date(15, 3, 2025), 1000.0, 0.0});
    
    auto& entries = wage.getEntries();
    ASSERT_EQ(entries.size(), 4);
    
    // Verify entries are sorted correctly
    EXPECT_EQ(entries[0].date, Date(15, 6, 2024));
    EXPECT_EQ(entries[1].date, Date(15, 12, 2024));
    EXPECT_EQ(entries[2].date, Date(15, 1, 2025));
    EXPECT_EQ(entries[3].date, Date(15, 3, 2025));
}

TEST_F(TestWage, CalculateStatisticsEmpty) {
    Wage wage("TestWage");
    
    WageStatistics stats = wage.calculateStatistics(Date(1, 1, 2025), Date(31, 12, 2025));
    
    EXPECT_EQ(stats.totalCheck, 0.0);
    EXPECT_EQ(stats.totalExtra, 0.0);
    EXPECT_EQ(stats.totalAmount, 0.0);
}

TEST_F(TestWage, CalculateStatisticsAll) {
    Wage wage("TestWage");
    
    wage.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    wage.addEntry({Date(15, 2, 2025), 1200.0, 100.0});
    wage.addEntry({Date(15, 3, 2025), 1100.0, 75.0});
    
    WageStatistics stats = wage.calculateStatistics(Date(1, 1, 2025), Date(31, 12, 2025));
    
    EXPECT_EQ(stats.totalCheck, 3300.0);
    EXPECT_EQ(stats.totalExtra, 225.0);
    EXPECT_EQ(stats.totalAmount, 3525.0);
}

TEST_F(TestWage, CalculateStatisticsDateRange) {
    Wage wage("TestWage");
    
    wage.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    wage.addEntry({Date(15, 2, 2025), 1200.0, 100.0});
    wage.addEntry({Date(15, 3, 2025), 1100.0, 75.0});
    wage.addEntry({Date(15, 4, 2025), 1300.0, 80.0});
    
    // Only February and March
    WageStatistics stats = wage.calculateStatistics(Date(1, 2, 2025), Date(31, 3, 2025));
    
    EXPECT_EQ(stats.totalCheck, 2300.0);
    EXPECT_EQ(stats.totalExtra, 175.0);
    EXPECT_EQ(stats.totalAmount, 2475.0);
}

TEST_F(TestWage, CalculateStatisticsPartialRange) {
    Wage wage("TestWage");
    
    wage.addEntry({Date(10, 3, 2025), 1000.0, 50.0});
    wage.addEntry({Date(15, 3, 2025), 1200.0, 100.0});
    wage.addEntry({Date(20, 3, 2025), 1100.0, 75.0});
    
    // Only entries from 15th onwards
    WageStatistics stats = wage.calculateStatistics(Date(15, 3, 2025), Date(31, 3, 2025));
    
    EXPECT_EQ(stats.totalCheck, 2300.0);
    EXPECT_EQ(stats.totalExtra, 175.0);
    EXPECT_EQ(stats.totalAmount, 2475.0);
}

TEST_F(TestWage, CalculateStatisticsNoMatch) {
    Wage wage("TestWage");
    
    wage.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    wage.addEntry({Date(15, 2, 2025), 1200.0, 100.0});
    
    // Range that doesn't include any entries
    WageStatistics stats = wage.calculateStatistics(Date(1, 6, 2025), Date(30, 6, 2025));
    
    EXPECT_EQ(stats.totalCheck, 0.0);
    EXPECT_EQ(stats.totalExtra, 0.0);
    EXPECT_EQ(stats.totalAmount, 0.0);
}

class TestWageManager: public ::testing::Test {

};

TEST_F(TestWageManager, CreateAndHasWage) {
    WageManager manager;
    
    EXPECT_FALSE(manager.hasWage("Salary"));
    
    manager.createWage("Salary");
    
    EXPECT_TRUE(manager.hasWage("Salary"));
}

TEST_F(TestWageManager, CreateMultipleWages) {
    WageManager manager;
    
    manager.createWage("Salary");
    manager.createWage("Freelance");
    manager.createWage("Bonus");
    
    EXPECT_TRUE(manager.hasWage("Salary"));
    EXPECT_TRUE(manager.hasWage("Freelance"));
    EXPECT_TRUE(manager.hasWage("Bonus"));
    EXPECT_FALSE(manager.hasWage("NonExistent"));
}

TEST_F(TestWageManager, DestroyWage) {
    WageManager manager;
    
    manager.createWage("Salary");
    manager.createWage("Freelance");
    
    EXPECT_TRUE(manager.hasWage("Salary"));
    EXPECT_TRUE(manager.hasWage("Freelance"));
    
    manager.destroyWage("Salary");
    
    EXPECT_FALSE(manager.hasWage("Salary"));
    EXPECT_TRUE(manager.hasWage("Freelance"));
}

TEST_F(TestWageManager, GetWage) {
    WageManager manager;
    
    manager.createWage("Salary");
    Wage& wage = manager.getWage("Salary");
    
    wage.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    
    auto& entries = wage.getEntries();
    ASSERT_EQ(entries.size(), 1);
    EXPECT_EQ(entries[0].amount, 1000.0);
}

TEST_F(TestWageManager, CalculateStatisticsEmpty) {
    WageManager manager;
    
    WageStatistics stats = manager.calculateStatistics(Date(1, 1, 2025), Date(31, 12, 2025));
    
    EXPECT_EQ(stats.totalCheck, 0.0);
    EXPECT_EQ(stats.totalExtra, 0.0);
    EXPECT_EQ(stats.totalAmount, 0.0);
}

TEST_F(TestWageManager, CalculateStatisticsSingleWage) {
    WageManager manager;
    
    manager.createWage("Salary");
    Wage& wage = manager.getWage("Salary");
    
    wage.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    wage.addEntry({Date(15, 2, 2025), 1200.0, 100.0});
    
    WageStatistics stats = manager.calculateStatistics(Date(1, 1, 2025), Date(31, 12, 2025));
    
    EXPECT_EQ(stats.totalCheck, 2200.0);
    EXPECT_EQ(stats.totalExtra, 150.0);
    EXPECT_EQ(stats.totalAmount, 2350.0);
}

TEST_F(TestWageManager, CalculateStatisticsMultipleWages) {
    WageManager manager;
    
    manager.createWage("Salary");
    manager.createWage("Freelance");
    manager.createWage("Bonus");
    
    Wage& salary = manager.getWage("Salary");
    salary.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    salary.addEntry({Date(15, 2, 2025), 1000.0, 50.0});
    
    Wage& freelance = manager.getWage("Freelance");
    freelance.addEntry({Date(10, 1, 2025), 500.0, 0.0});
    freelance.addEntry({Date(20, 2, 2025), 750.0, 50.0});
    
    Wage& bonus = manager.getWage("Bonus");
    bonus.addEntry({Date(1, 2, 2025), 300.0, 0.0});
    
    WageStatistics stats = manager.calculateStatistics(Date(1, 1, 2025), Date(28, 2, 2025));
    
    EXPECT_EQ(stats.totalCheck, 3550.0);
    EXPECT_EQ(stats.totalExtra, 150.0);
    EXPECT_EQ(stats.totalAmount, 3700.0);
}

TEST_F(TestWageManager, CalculateStatisticsWithDateFilter) {
    WageManager manager;
    
    manager.createWage("Salary");
    manager.createWage("Freelance");
    
    Wage& salary = manager.getWage("Salary");
    salary.addEntry({Date(15, 1, 2025), 1000.0, 50.0});
    salary.addEntry({Date(15, 2, 2025), 1000.0, 50.0});
    salary.addEntry({Date(15, 3, 2025), 1000.0, 50.0});
    
    Wage& freelance = manager.getWage("Freelance");
    freelance.addEntry({Date(10, 1, 2025), 500.0, 0.0});
    freelance.addEntry({Date(20, 2, 2025), 750.0, 50.0});
    
    // Only February entries
    WageStatistics stats = manager.calculateStatistics(Date(1, 2, 2025), Date(28, 2, 2025));
    
    EXPECT_EQ(stats.totalCheck, 1750.0);
    EXPECT_EQ(stats.totalExtra, 100.0);
    EXPECT_EQ(stats.totalAmount, 1850.0);
}
