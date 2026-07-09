#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class AddPaycheckPopup : public App::Widgets::Popup {
    public:
        explicit AddPaycheckPopup(App::AppData& appData, Store& store);

        ~AddPaycheckPopup() override = default;

        void setWageName(const std::string& wageName);

    protected:

        void drawContent(Core::Window &window, Core::Context &context) override;

        void reset() override;

    private:
        Store& m_Store;

        double m_InputCheck = 0.0;
        double m_InputExtra = 0.0;
        Date m_InputDate = getCurrentDate();

        std::string m_WageName;
    };

}