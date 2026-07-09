#pragma once

#include <VelyraAppFramework/Application.hpp>

VL_ENUM(FINMGR_APP_LAYOUTS, Velyra::App::LayoutID,
    FINMGR_APP_OVERVIEW_LAYOUT = 1,
    FINMGR_APP_CONFIGURE_FINANCE_LAYOUT = 2
);

VL_ENUM(FINMGR_APP_POPUPS, Velyra::App::Widgets::PopupID,
    FINMGR_APP_POPUP_CREATE_INCOME = 1,
    FINMGR_APP_POPUP_ADD_PAYCHECK = 2,
    FINMGR_APP_POPUP_CREATE_SAVINGS_RECORD = 3,
    FINMGR_APP_POPUP_ADD_SAVINGS_ENTRY = 4,
    FINMGR_APP_POPUP_CREATE_MONTHLY_EXPENSE = 5,
    FINMGR_APP_POPUP_ADD_MONTHLY_EXPENSE = 6
);

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

    /**
     *
     * @Brief Results for the Ordinary Least Squares (OLS) linear regression.
     */
    struct OLSLinearRegression {
        double slope = 0.0;
        double intercept = 0.0;
        double rSquared = 0.0;
    };

    OLSLinearRegression performOLSLinearRegression(const std::vector<double>& x, const std::vector<double>& y);

}