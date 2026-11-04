#pragma once

#include <FinanceManager/FinanceData.hpp>

namespace FinMgr {

    class AddPaycheckPopup;
    class AddSavingsEntryPopup;
    class AddExpenseEntryPopup;
    class CreateIncomePopup;
    class CreateSavingsRecordPopup;
    class CreateMonthlyExpensePopup;
    class ConfirmPopup;
    class SettingsPopup;

    enum class Page: int {
        Dashboard = 0,
        Income,
        Savings,
        Expenses,
        Manage,
        Count
    };

    const char* getPageName(Page page);

    /**
     * @brief Everything about how the app looks, saved per PC (never synced with git) under the "FinanceManagerUI" key
     */
    struct UiState {
        int page = static_cast<int>(Page::Dashboard);
        bool showSidebar = true;
        bool showCharts = true;
        std::string selectedWage;     // Empty shows the first one
        std::string selectedSavings;  // Empty shows all accounts combined
        int chartMonths = 12;         // Months shown in the dashboard charts, 0 for all
        std::string currency;         // Shown after every amount, e.g. "€"

        Page getPage() const { return static_cast<Page>(page); }

        void setPage(const Page newPage) { page = static_cast<int>(newPage); }

        VL_GENERATE_JSON_SERIALIZER_RELAXED(UiState, page, showSidebar, showCharts, selectedWage, selectedSavings, chartMonths, currency)
    };

    struct Popups {
        SP<AddPaycheckPopup> addPaycheck;
        SP<AddSavingsEntryPopup> addSavingsEntry;
        SP<AddExpenseEntryPopup> addExpenseEntry;
        SP<CreateIncomePopup> createIncome;
        SP<CreateSavingsRecordPopup> createSavingsRecord;
        SP<CreateMonthlyExpensePopup> createMonthlyExpense;
        SP<ConfirmPopup> confirm;
        SP<SettingsPopup> settings;
    };

    struct Store {
        FinanceData data;
        UiState ui;
        Popups popups;
    };

}
