#include <gtest/gtest.h>
#include <FinanceManager/Date.hpp>

using namespace FinMgr;

class TestDate: public ::testing::Test {

};

TEST_F(TestDate, toInteger) {
    Date b1(17, 12, 2025);
    Date b2(15, 12, 2025);
    Date b3(24, 01, 1970);

    EXPECT_EQ(b1.toInteger(), 20251217);
    EXPECT_EQ(b2.toInteger(), 20251215);
    EXPECT_EQ(b3.toInteger(), 19700124);
}

TEST_F(TestDate, YearIncrement) {
    Date b1(17, 12, 1971);
    Date b2(15, 12, 2025);

    b1.yearIncrement();
    b2.yearIncrement();

    EXPECT_EQ(b1, Date(17, 12, 1972));
    EXPECT_EQ(b2, Date(15, 12, 2026));
}

TEST_F(TestDate, MonthIncrement) {
    Date b1(17, 8, 1971);
    Date b2(15, 12, 2025);

    b1.monthIncrement();
    b2.monthIncrement();

    EXPECT_EQ(b1, Date(17, 9, 1971));
    EXPECT_EQ(b2, Date(15, 1, 2026));
}

TEST_F(TestDate, DayIncrement) {
    Date b1(17, 8, 1971);
    b1.dayIncrement();
    EXPECT_EQ(b1, Date(18, 8, 1971));

    // Month with 31 days roll around
    Date b2(29, 1, 2022);
    b2.dayIncrement();
    EXPECT_EQ(b2, Date(30, 1, 2022));
    b2.dayIncrement();
    b2.dayIncrement();
    EXPECT_EQ(b2, Date(1, 2, 2022));

    // Month with 30 days roll around
    Date b3(29, 4, 2022);
    b3.dayIncrement();
    EXPECT_EQ(b3, Date(30, 4, 2022));
    b3.dayIncrement();
    EXPECT_EQ(b3, Date(1, 5, 2022));

    // Year roll around
    Date b4(29, 12, 2022);
    b4.dayIncrement();
    EXPECT_EQ(b4, Date(30, 12, 2022));
    b4.dayIncrement();
    b4.dayIncrement();
    EXPECT_EQ(b4, Date(1, 1, 2023));

    // February normal year
    Date b5(27, 2, 2022);
    b5.dayIncrement();
    EXPECT_EQ(b5, Date(28, 2, 2022));
    b5.dayIncrement();
    EXPECT_EQ(b5, Date(1, 3, 2022));

    // February leap year
    Date b6(27, 2, 2020);
    b6.dayIncrement();
    EXPECT_EQ(b6, Date(28, 2, 2020));
    b6.dayIncrement();
    b6.dayIncrement();
    EXPECT_EQ(b6, Date(1, 3, 2020));
}

TEST_F(TestDate, SmallerThan) {
    const Date d1(1, 10, 2022);
    const Date d2(1, 2, 2023);
    EXPECT_TRUE(d1 < d2);

    const Date d3(1, 2, 2022);
    const Date d4(1, 3, 2022);
    EXPECT_TRUE(d3 < d4);
    EXPECT_FALSE(d2 < d3);

    const Date d5(1, 2, 2022);
    const Date d6(2, 2, 2022);
    EXPECT_TRUE(d5 < d6);
}

TEST_F(TestDate, GreaterThan) {
    const Date d1(1, 2, 2023);
    const Date d2(1, 10, 2022);
    EXPECT_TRUE(d1 > d2);

    const Date d3(1, 3, 2022);
    const Date d4(1, 2, 2022);
    EXPECT_TRUE(d3 > d4);
    EXPECT_FALSE(d4 > d3);

    const Date d5(2, 2, 2022);
    const Date d6(1, 2, 2022);
    EXPECT_TRUE(d5 > d6);
}

TEST_F(TestDate, LessThanOrEqual) {
    const Date d1(1, 10, 2022);
    const Date d2(1, 2, 2023);
    EXPECT_TRUE(d1 <= d2);

    const Date d3(1, 2, 2022);
    const Date d4(1, 2, 2022);
    EXPECT_TRUE(d3 <= d4);  // Equal case
    
    const Date d5(1, 3, 2022);
    const Date d6(1, 2, 2022);
    EXPECT_FALSE(d5 <= d6);
}

TEST_F(TestDate, GreaterThanOrEqual) {
    const Date d1(1, 2, 2023);
    const Date d2(1, 10, 2022);
    EXPECT_TRUE(d1 >= d2);

    const Date d3(1, 2, 2022);
    const Date d4(1, 2, 2022);
    EXPECT_TRUE(d3 >= d4);  // Equal case
    
    const Date d5(1, 2, 2022);
    const Date d6(1, 3, 2022);
    EXPECT_FALSE(d5 >= d6);
}

TEST_F(TestDate, LeapYearEdgeCases) {
    // Year 2000 is a leap year (divisible by 400)
    Date d1(28, 2, 2000);
    d1.dayIncrement();
    EXPECT_EQ(d1, Date(29, 2, 2000));
    d1.dayIncrement();
    EXPECT_EQ(d1, Date(1, 3, 2000));

    // Year 1900 is NOT a leap year (divisible by 100 but not 400)
    Date d2(28, 2, 1900);
    d2.dayIncrement();
    EXPECT_EQ(d2, Date(1, 3, 1900));

    // Year 2100 is NOT a leap year (divisible by 100 but not 400)
    Date d3(28, 2, 2100);
    d3.dayIncrement();
    EXPECT_EQ(d3, Date(1, 3, 2100));

    // Year 2024 is a leap year (divisible by 4, not by 100)
    Date d4(28, 2, 2024);
    d4.dayIncrement();
    EXPECT_EQ(d4, Date(29, 2, 2024));
    d4.dayIncrement();
    EXPECT_EQ(d4, Date(1, 3, 2024));
}

TEST_F(TestDate, ForLoopIteration) {

    const Date start(1, 10, 2022);
    const Date end(2, 2, 2023);

    std::vector<Date> dates;
    for (Date date = start; date < end; date.monthIncrement()) {
        dates.push_back(date);
    }

    ASSERT_EQ(dates.size(), 5);
    EXPECT_EQ(dates[0], Date(1, 10, 2022));
    EXPECT_EQ(dates[1], Date(1, 11, 2022));
    EXPECT_EQ(dates[2], Date(1, 12, 2022));
    EXPECT_EQ(dates[3], Date(1, 1, 2023));
    EXPECT_EQ(dates[4], Date(1, 2, 2023));
}

TEST_F(TestDate, ConstructionOrderValidation) {
    // Common mistake: passing year first instead of day
    Date d1(15, 4, 2026);  // April 15, 2026
    EXPECT_EQ(d1.day, 15);
    EXPECT_EQ(d1.month, 4);
    EXPECT_EQ(d1.year, 2026);
}