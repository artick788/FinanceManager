#include "MonthlyExpenseOverview.hpp"

#include "Utils.hpp"

#include "Popups/AddMonthlyExpensePopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    MonthlyExpenseOverview::MonthlyExpenseOverview(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store) {

    }

    void MonthlyExpenseOverview::drawStatistics() const {

    }

    void MonthlyExpenseOverview::drawDetail() {
        if (ImGui::Button("Add Monthly Expense")) {
            m_Store.addMonthlyExpensePopup->setOpen(true);
        }
        drawTable();
    }

    void MonthlyExpenseOverview::drawTable() {

    }
}
