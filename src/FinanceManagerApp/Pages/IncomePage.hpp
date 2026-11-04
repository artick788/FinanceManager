#pragma once

#include "../Store.hpp"

#include <FinanceManager/Reports.hpp>
#include <VelyraAppFramework/LayoutEngine/LayoutDescription.hpp>

namespace FinMgr {

    class IncomePage {
    public:
        explicit IncomePage(Store& store);

        SP<App::Node> buildLayout();

        /**
         * @return The selected wage, the first one if nothing valid is selected, nullptr if there are no wages
         */
        const std::string* getSelectedWage() const;

    private:
        void drawTable();

        void drawChart();

        void drawTotalRow(const char* label, const WageTotals& totals, const ImVec4& background);

        void drawEntryRow(const std::string& wageName, WageEntry& entry);

    private:
        Store& m_Store;
    };

}
