#pragma once

#include <VelyraAppFramework/Widgets/Panel.hpp>
#include "../Store.hpp"

namespace FinMgr {

    class SavingsTrendPanel : public App::Widgets::Panel {
    public:
        SavingsTrendPanel(App::AppData& appData, SavingsRecord& record);

        ~SavingsTrendPanel() override = default;

    protected:
        void drawContent() override;

    private:
        void createSavingsData(const SavingsRecord& record);

    private:
        std::vector<double> m_Times;
        std::vector<double> m_ExpectedSavings;
        std::vector<double> m_ActualSavings;

    };
}
