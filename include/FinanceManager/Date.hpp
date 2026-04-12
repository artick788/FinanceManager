#pragma once

#include <FinanceManager/FinanceManagerInclude.hpp>

namespace FinMgr {

    struct Date {
        int day = 0;
        int month = 0;
        int year = 0;

        Date() = default;

        Date(int day_, int month_, int year_);

        void dayIncrement();

        void monthIncrement();

        void yearIncrement();

        bool operator==(const Date &) const;

        bool operator!=(const Date &) const;

        bool operator<(const Date &) const;

        bool operator<=(const Date &) const;

        std::string toString() const;

        U64 toInteger() const;

        VL_GENERATE_JSON_SERIALIZER(Date, day, month, year)
    };

}