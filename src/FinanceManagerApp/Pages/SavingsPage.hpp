#pragma once

#include "../Store.hpp"

#include <FinanceManager/Reports.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace FinMgr {

    class SavingsPage {
    public:
        explicit SavingsPage(Store& store);

        SP<App::Node> buildLayout();

        /**
         * @return The selected record, nullptr when all accounts are shown combined
         */
        const std::string* getSelectedRecord() const;

    private:
        void drawOverview();

        void drawRecordTable();

        void drawChart();

        void drawTotalRow(const char* label, const char* dateLabel, double deposited, double expected, double actual, double change, const ImVec4& background, const std::string& changeTooltip);

        void drawEntryRow(const std::string& recordName, const SavingsRow& row);

        void drawTrendPlot(const char* id, const SavingsTrend& trend);

    private:
        Store& m_Store;
    };

}
