#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class AddMonthlyExpensePopup : public App::Widgets::Popup {
    public:
        explicit AddMonthlyExpensePopup(App::AppData& appData, Store& store);

        ~AddMonthlyExpensePopup() override = default;

    protected:

        void drawContent(Core::Window &window, Core::Context &context) override;

        void reset() override;

    private:

        void drawMonthlyExpenseSelector();

        void drawAddAEntry();

        void drawButtons();

    private:
        Store& m_Store;

        std::string m_SelectedExpenseName;
        Date m_InputDate = getCurrentDate();
        double m_InputAmount = 0.0;

        bool m_FullyFilled = false;
    };

}