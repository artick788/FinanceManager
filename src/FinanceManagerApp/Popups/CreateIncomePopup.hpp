#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class CreateIncomePopup : public App::Widgets::Popup {
    public:
        explicit CreateIncomePopup(App::AppData& appData, Store& store);

        ~CreateIncomePopup() override = default;

    protected:

        void drawContent(Core::Window &window, Core::Context &context) override;

        void reset() override;

    private:
        Store& m_Store;

        std::string m_WageName;
    };

}