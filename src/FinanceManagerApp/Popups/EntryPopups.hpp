#pragma once

#include "ConfirmPopup.hpp"
#include "../Store.hpp"

#include <FinanceManager/Reports.hpp>

namespace FinMgr {

    /**
     * @brief Shared layout of the popups that add an entry: a source selector, a form and confirmation buttons
     */
    class EntryPopup: public App::Widgets::Popup {
    public:
        EntryPopup(App::AppData& appData, Store& store, const std::string& name, App::Widgets::PopupID id);

        ~EntryPopup() override = default;

    protected:
        /**
         * @brief Combo to pick one of the given names
         * @return true if the selection changed
         */
        static bool drawSourceSelector(const char* label, const std::vector<std::string>& names, std::string& selected);

        static void drawWarning(const std::string& warning);

    protected:
        Store& m_Store;
    };

    class AddPaycheckPopup: public EntryPopup {
    public:
        AddPaycheckPopup(App::AppData& appData, Store& store);

        void open(const std::string& wageName);

    protected:
        void drawContent() override;

        void onConfirm() override;

        void reset() override;

    private:
        void selectWage(const std::string& name);

    private:
        std::string m_WageName;
        Date m_Date = getCurrentDate();
        double m_Amount = 0.0;
        double m_Extra = 0.0;
    };

    class AddSavingsEntryPopup: public EntryPopup {
    public:
        AddSavingsEntryPopup(App::AppData& appData, Store& store);

        void open(const std::string& recordName);

    protected:
        void drawContent() override;

        void onConfirm() override;

        void reset() override;

    private:
        void selectRecord(const std::string& name);

    private:
        std::string m_RecordName;
        Date m_Date = getCurrentDate();
        double m_Deposit = 0.0;
        double m_ActualState = 0.0;
    };

    class AddExpenseEntryPopup: public EntryPopup {
    public:
        AddExpenseEntryPopup(App::AppData& appData, Store& store);

        /**
         * @param expenseName The expense to add an entry for, empty to let the user pick
         * @param month The month of the entry, the current month if not given
         */
        void open(const std::string& expenseName, std::optional<YearMonth> month = std::nullopt);

    protected:
        void drawContent() override;

        void onConfirm() override;

        void reset() override;

    private:
        void selectExpense(const std::string& name);

    private:
        std::string m_ExpenseName;
        Date m_Date = getCurrentDate();
        double m_Amount = 0.0;
    };

}
