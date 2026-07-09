#pragma once

#include <VelyraAppFramework/Application.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Store.hpp"

namespace FinMgr {

    class CreateSavingsRecordPopup : public App::Widgets::Popup {
    public:
        explicit CreateSavingsRecordPopup(App::AppData& app_data, Store& store);

        ~CreateSavingsRecordPopup() override = default;

    protected:

        void drawContent(Core::Window &window, Core::Context &context) override;

        void reset() override;

    private:
        Store& m_Store;

        std::string m_RecordName;
        double m_InputDefaultTransactionAmount = 0.0;
        double m_InputInitialAmount = 0.0;
    };

}