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

        bool operator>(const Date &) const;

        bool operator<=(const Date &) const;

        bool operator>=(const Date&) const;

        std::string toString() const;

        U64 toInteger() const;

        void fromInteger(U64 value);

        void fromJson(const nlohmann::json& j);

        nlohmann::json toJson() const;

        double toUnixTimestamp() const;
    };

    Date getCurrentDate();

}