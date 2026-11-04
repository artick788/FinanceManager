#pragma once

#include "ConfirmPopup.hpp"
#include "../Store.hpp"

namespace FinMgr {

    class CreatePopup: public App::Widgets::Popup {
    public:
        CreatePopup(App::AppData& appData, Store& store, const std::string& name, App::Widgets::PopupID id);

        ~CreatePopup() override = default;

        void open();

    protected:
        /**
         * @brief Draws the name input with validation
         * @return true if the name is valid
         */
        bool drawNameInput(const char* hint, bool exists);

    protected:
        Store& m_Store;
        std::string m_Name;
    };

    class CreateIncomePopup: public CreatePopup {
    public:
        CreateIncomePopup(App::AppData& appData, Store& store);

    protected:
        void drawContent() override;

        void onConfirm() override;

        void reset() override;
    };

    class CreateSavingsRecordPopup: public CreatePopup {
    public:
        CreateSavingsRecordPopup(App::AppData& appData, Store& store);

    protected:
        void drawContent() override;

        void onConfirm() override;

        void reset() override;

    private:
        double m_InitialAmount = 0.0;
        double m_DefaultDeposit = 0.0;
    };

    class CreateMonthlyExpensePopup: public CreatePopup {
    public:
        CreateMonthlyExpensePopup(App::AppData& appData, Store& store);

    protected:
        void drawContent() override;

        void onConfirm() override;

        void reset() override;

    private:
        int m_Day = 1;
        double m_Amount = 0.0;
        bool m_StartThisMonth = true;
    };

}
