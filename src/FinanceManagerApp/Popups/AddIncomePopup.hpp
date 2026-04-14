#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class AddIncomePopup : public App::Widgets::Popup {
    public:
        explicit AddIncomePopup(Store& store);

        ~AddIncomePopup() override = default;

    protected:

        void drawContent() override;

        void reset() override;

    private:
        Store& m_Store;

        std::string m_WageName;
    };

}