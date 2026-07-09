#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class AddSavingsEntryPopup : public App::Widgets::Popup {
    public:
        explicit AddSavingsEntryPopup(App::AppData& appData, Store& store);

        ~AddSavingsEntryPopup() override = default;

        void setSavingsName(const std::string& savingsName);

    protected:

        void drawContent(Core::Window &window, Core::Context &context) override;

        void reset() override;

    private:
        Store& m_Store;

        double m_InputAmount = 0.0;
        double m_InputCurrentState = 0.0;
        Date m_InputDate = getCurrentDate();

        std::string m_SavingsName;
    };
}