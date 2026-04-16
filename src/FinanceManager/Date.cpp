#include <FinanceManager/Date.hpp>

#include <unordered_set>

namespace FinMgr {

    static const std::unordered_set<int> s_31 = {1, 3, 5, 7, 8, 10, 12};
    static const std::unordered_set<int> s_30 = {4, 6, 9, 11};

    Date::Date(const int day_, const int month_, const int year_) :
    day(day_), month(month_), year(year_) {
    }

    void Date::dayIncrement() {
        day++;

        if (s_31.find(month) != s_31.end()) {
            if (day > 31) {
                day = 1;
                monthIncrement();
            }
        }
        else if (s_30.find(month) != s_30.end()) {
            if (day > 30) {
                day = 1;
                monthIncrement();
            }
        }
        else {
            // current month is february
            // Correct leap year calculation: divisible by 4, except century years unless divisible by 400
            const bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if (isLeapYear) {
                if (day > 29) {
                    day = 1;
                    monthIncrement();
                }
            }
            else {
                if (day > 28) {
                    day = 1;
                    monthIncrement();
                }
            }
        }
    }

    void Date::monthIncrement() {
        month++;
        if (month > 12) {
            month = 1;
            yearIncrement();
        }
    }

    void Date::yearIncrement() {
        year++;
    }

    bool Date::operator==(const Date &other) const {
        return (year == other.year && month == other.month && day == other.day);
    }

    bool Date::operator!=(const Date &other) const {
        return !(*this == other);
    }

    bool Date::operator<(const Date &other) const {
        return toInteger() < other.toInteger();
    }

    bool Date::operator>(const Date &other) const {
        return toInteger() > other.toInteger();
    }

    bool Date::operator<=(const Date &other) const {
        return (*this == other) || (*this < other);
    }

    bool Date::operator>=(const Date& other) const {
        return (*this == other) || (*this > other);
    }

    std::string Date::toString() const {
        return std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
    }

    U64 Date::toInteger() const {
        return static_cast<U64>(year) * 10000 +
           static_cast<U64>(month) * 100 +
           static_cast<U64>(day);
    }

    void Date::fromInteger(const U64 value) {
        year = static_cast<int>(value / 10000);
        month = static_cast<int>((value % 10000) / 100);
        day = static_cast<int>(value % 100);
    }

    void Date::fromJson(const nlohmann::json &j) {
        fromInteger(j.get<U64>());
    }

    nlohmann::json Date::toJson() const {
        return toInteger();
    }

    Date getCurrentDate() {
        const auto now = std::chrono::system_clock::now();
        const std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        const std::tm* now_tm = std::localtime(&now_c);
        return Date(now_tm->tm_mday, now_tm->tm_mon + 1, now_tm->tm_year + 1900);
    }

}