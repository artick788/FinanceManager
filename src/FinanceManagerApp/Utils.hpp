#pragma once

#include <VelyraAppFramework/Application.hpp>

namespace FinMgr {

    inline void tableEmptyRow(const Size columnCount) {
        ImGui::TableNextRow();
        for (Size col = 0; col < columnCount; ++col) {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("");
        }
    }

    inline const char* getMonthStr(const int month) {
        static const char* months[] = {
            "January",
            "February",
            "March",
            "April",
            "May",
            "June",
            "July",
            "August",
            "September",
            "October",
            "November",
            "December"
        };
        return months[month - 1];
    }

}