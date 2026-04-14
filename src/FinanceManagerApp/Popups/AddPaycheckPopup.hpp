#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class AddPaycheckPopup : public App::Widgets::Popup {
    public:
        explicit AddPaycheckPopup(Store& store);

        ~AddPaycheckPopup() override = default;

        void setWageName(const std::string& wageName) {
            m_WageName = wageName;
        }

    protected:

        void drawContent() override;

        void reset() override;

    private:
        Store& m_Store;

        double m_InputCheck = 0.0;
        double m_InputExtra = 0.0;
        Date m_InputDate = getCurrentDate();

        std::string m_WageName;
    };

}