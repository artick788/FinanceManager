#pragma once

#include <VelyraAppFramework/Widgets/Popup.hpp>

#include <functional>
#include <string>

namespace FinMgr {

    using namespace Velyra;

    enum FinanceManagerPopupID: App::Widgets::PopupID {
        FINMGR_POPUP_CONFIRM = 1,
        FINMGR_POPUP_ADD_PAYCHECK,
        FINMGR_POPUP_ADD_SAVINGS_ENTRY,
        FINMGR_POPUP_ADD_EXPENSE_ENTRY,
        FINMGR_POPUP_CREATE_INCOME,
        FINMGR_POPUP_CREATE_SAVINGS_RECORD,
        FINMGR_POPUP_CREATE_MONTHLY_EXPENSE,
        FINMGR_POPUP_SETTINGS
    };

    /**
     * @brief Asks the user to confirm a destructive action, e.g. deleting an income source with all its paychecks
     */
    class ConfirmPopup: public App::Widgets::Popup {
    public:
        explicit ConfirmPopup(App::AppData& appData);

        ~ConfirmPopup() override = default;

        void ask(const std::string& message, const std::string& confirmLabel, std::function<void()> action);

    protected:
        void drawContent() override;

        void reset() override;

    private:
        std::string m_Message;
        std::string m_ConfirmLabel;
        std::function<void()> m_Action;
    };

}
