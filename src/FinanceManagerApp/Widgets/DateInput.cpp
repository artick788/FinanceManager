#include "DateInput.hpp"

#include <unordered_set>

namespace FinMgr::Widgets {

    static const std::unordered_set<int> s_31 = {1, 3, 5, 7, 8, 10, 12};
    static const std::unordered_set<int> s_30 = {4, 6, 9, 11};

    void checkDay(Date& date) {
        if (date.day < 1) {
            date.month--;
            // depending on the current month, the  boundary of the dates are different
            if (s_31.find(date.month) != s_31.end()) {
                date.day = 31;
            }
            else if (s_30.find(date.month) != s_30.end()) {
                date.day = 30;
            }
            else if (date.month == 2) {
                // February has jump days...
                // TODO, every 100 years, we skip a jump day except every 400 years
                if (date.year % 4 == 0) {
                    date.day = 29;
                }
                else {
                    date.day = 28;
                }
            }
            else {
                // We matched no month in the sets, meaning we decremented the month counter below 1,
                // so we should be in december
                date.day = 31;
            }
        }
        // depending on the current month, the upper boundary of the dates are different
        if (s_31.find(date.month) != s_31.end()) {
            if (date.day > 31) {
                date.day = 1;
                date.month++;
            }
        }
        else if (s_30.find(date.month) != s_30.end()) {
            if (date.day > 30) {
                date.day = 1;
                date.month++;
            }
        }
        else {
            // February has jump days...
            // TODO, every 100 years, we skip a jump day except every 400 years
            if (date.year % 4 == 0) {
                if (date.day > 29) {
                    date.day = 1;
                    date.month++;
                }
            }
            else {
                if (date.day > 28) {
                    date.day = 1;
                    date.month++;
                }
            }
        }
    }

    void checkMonth(Date& date) {
        if (date.month < 1) {
            date.month = 12;
            date.year--;
        }
        else if (date.month > 12) {
            date.month = 1;
            date.year++;
        }
    }

    void checkYear(Date& date) {
        if (date.year < 1970) {
            date.year = 1970;
        }
    }

    void DateInput(Date &date, const bool withDayInput) {
        int checked = ImGui::InputInt("Year", &date.year);
        checked += ImGui::InputInt("Month", &date.month);
        if (withDayInput) {
            checked += ImGui::InputInt("Day", &date.day);
        }

        if (checked) {
            if (withDayInput) {
                checkDay(date);
            }
            checkMonth(date);
            checkYear(date);
        }
    }
}