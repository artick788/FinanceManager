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
            if (year % 4 == 0) {
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

    bool Date::operator<=(const Date &other) const {
        return (*this == other) || (*this < other);
    }

    std::string Date::toString() const {
        return std::to_string(day) + "/" + std::to_string(month) + "/" + std::to_string(year);
    }

    U64 Date::toInteger() const {
        return static_cast<U64>(year) * 10000 +
           static_cast<U64>(month) * 100 +
           static_cast<U64>(day);
    }

}