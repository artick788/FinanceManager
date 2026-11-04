#include "EntryPopups.hpp"

#include "../Theme.hpp"
#include "../Ui.hpp"

#include <VelyraAppFramework/AppData.hpp>

#include <ranges>

namespace FinMgr {

    namespace {

        constexpr float FORM_WIDTH = 420.0f;

        template<typename MAP>
        std::vector<std::string> sortedNames(const MAP& map) {
            std::vector<std::string> names;
            for (const auto& name: map | std::views::keys) {
                names.push_back(name);
            }
            std::ranges::sort(names);
            return names;
        }

        void formWidth() {
            ImGui::Dummy(ImVec2(FORM_WIDTH * ImGui::GetStyle().FontScaleDpi, 0.0f));
        }

        template<typename ENTRIES>
        bool hasEntryInMonth(const ENTRIES& entries, const YearMonth month) {
            return std::ranges::any_of(entries, [month](const auto& entry) { return month.contains(entry.date); });
        }

    }

    EntryPopup::EntryPopup(App::AppData& appData, Store& store, const std::string& name, const App::Widgets::PopupID id):
    Popup(appData, name, id),
    m_Store(store) {

    }

    bool EntryPopup::drawSourceSelector(const char* label, const std::vector<std::string>& names, std::string& selected) {
        bool changed = false;
        Ui::formLabel(label);
        if (ImGui::BeginCombo("##Source", selected.empty() ? "Select..." : selected.c_str())) {
            for (const auto& name: names) {
                if (ImGui::Selectable(name.c_str(), name == selected)) {
                    changed = name != selected;
                    selected = name;
                }
            }
            ImGui::EndCombo();
        }
        return changed;
    }

    void EntryPopup::drawWarning(const std::string& warning) {
        if (!warning.empty()) {
            ImGui::TextColored(Theme::Warning, "%s", warning.c_str());
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    AddPaycheckPopup::AddPaycheckPopup(App::AppData& appData, Store& store):
    EntryPopup(appData, store, "Add paycheck", FINMGR_POPUP_ADD_PAYCHECK) {

    }

    void AddPaycheckPopup::open(const std::string& wageName) {
        reset();
        selectWage(wageName);
        m_AppData.openPopup(getID());
    }

    void AddPaycheckPopup::selectWage(const std::string& name) {
        m_WageName = name;
        if (!m_Store.data.wageManager.hasWage(name)) {
            return;
        }
        // Most paychecks are the same as the previous one, so start from that
        const auto& entries = m_Store.data.wageManager.getWage(name).getEntries();
        if (!entries.empty()) {
            m_Amount = entries.back().amount;
            m_Date.day = std::min(entries.back().date.day, daysInMonth(m_Date.month, m_Date.year));
        }
    }

    void AddPaycheckPopup::drawContent() {
        formWidth();
        auto& wages = m_Store.data.wageManager;
        if (drawSourceSelector("Income source", sortedNames(wages.getWages()), m_WageName)) {
            selectWage(m_WageName);
        }
        Ui::formLabel("Date");
        Ui::dateInput("##Date", m_Date);
        Ui::formLabel("Amount");
        ImGui::InputDouble("##Amount", &m_Amount, 10.0, 100.0, "%.2f");
        Ui::formLabel("Extra");
        ImGui::InputDouble("##Extra", &m_Extra, 10.0, 100.0, "%.2f");
        ImGui::SameLine();
        Ui::helpMarker("Bonuses, holiday pay, reimbursements... Anything on top of the regular paycheck.");

        ImGui::Separator();
        ImGui::Text("Total");
        ImGui::SameLine(170.0f * ImGui::GetStyle().FontScaleDpi);
        ImGui::TextColored(Theme::Positive, "%s", Ui::money(m_Amount + m_Extra).c_str());

        const bool valid = wages.hasWage(m_WageName) && m_Amount + m_Extra > 0.0;
        if (valid && hasEntryInMonth(wages.getWage(m_WageName).getEntries(), YearMonth::of(m_Date))) {
            drawWarning("There already is a paycheck in " + Ui::monthYear(m_Date.month, m_Date.year));
        }
        ImGui::Spacing();
        drawConfirmationButtons("Add paycheck", "Cancel", valid);
    }

    void AddPaycheckPopup::onConfirm() {
        m_Store.data.wageManager.getWage(m_WageName).addEntry({m_Date, m_Amount, m_Extra});
        m_Store.ui.selectedWage = m_WageName;
    }

    void AddPaycheckPopup::reset() {
        m_WageName.clear();
        m_Date = getCurrentDate();
        m_Amount = 0.0;
        m_Extra = 0.0;
    }

    // -----------------------------------------------------------------------------------------------------------------

    AddSavingsEntryPopup::AddSavingsEntryPopup(App::AppData& appData, Store& store):
    EntryPopup(appData, store, "Add savings entry", FINMGR_POPUP_ADD_SAVINGS_ENTRY) {

    }

    void AddSavingsEntryPopup::open(const std::string& recordName) {
        reset();
        selectRecord(recordName);
        m_AppData.openPopup(getID());
    }

    void AddSavingsEntryPopup::selectRecord(const std::string& name) {
        m_RecordName = name;
        if (!m_Store.data.savingsManager.hasSavingsRecord(name)) {
            return;
        }
        const SavingsRecord& record = m_Store.data.savingsManager.getSavingsRecord(name);
        m_Deposit = record.defaultTransactionAmount;
        const double previous = record.getEntries().empty() ? record.initialAmount : record.getEntries().back().actualState;
        m_ActualState = previous + m_Deposit;
    }

    void AddSavingsEntryPopup::drawContent() {
        formWidth();
        auto& savings = m_Store.data.savingsManager;
        if (drawSourceSelector("Account", sortedNames(savings.getSavingsRecords()), m_RecordName)) {
            selectRecord(m_RecordName);
        }
        Ui::formLabel("Date");
        Ui::dateInput("##Date", m_Date);
        Ui::formLabel("Deposit");
        ImGui::InputDouble("##Deposit", &m_Deposit, 10.0, 100.0, "%.2f");
        ImGui::SameLine();
        Ui::helpMarker("The amount you planned to put on the account, used to calculate the expected balance.");
        Ui::formLabel("Balance");
        ImGui::InputDouble("##Balance", &m_ActualState, 10.0, 100.0, "%.2f");
        ImGui::SameLine();
        Ui::helpMarker("The balance of the account at this date, as shown by your bank.");

        const bool valid = savings.hasSavingsRecord(m_RecordName);
        if (valid) {
            SavingsRecord& record = savings.getSavingsRecord(m_RecordName);
            const SavingsTable table = buildSavingsTable(record);
            const double expected = table.expected + m_Deposit;
            const double variance = m_ActualState - expected;
            ImGui::Separator();
            ImGui::TextDisabled("Expected balance");
            ImGui::SameLine(170.0f * ImGui::GetStyle().FontScaleDpi);
            ImGui::Text("%s", Ui::money(expected).c_str());
            ImGui::TextDisabled("Variance");
            ImGui::SameLine(170.0f * ImGui::GetStyle().FontScaleDpi);
            ImGui::TextColored(variance >= 0.0 ? Theme::Positive : Theme::Negative, "%s", Ui::signedMoney(variance).c_str());
            if (hasEntryInMonth(record.getEntries(), YearMonth::of(m_Date))) {
                drawWarning("There already is an entry in " + Ui::monthYear(m_Date.month, m_Date.year));
            }
        }
        ImGui::Spacing();
        drawConfirmationButtons("Add entry", "Cancel", valid);
    }

    void AddSavingsEntryPopup::onConfirm() {
        m_Store.data.savingsManager.getSavingsRecord(m_RecordName).addEntry(m_Date, m_Deposit, m_ActualState);
        m_Store.ui.selectedSavings = m_RecordName;
    }

    void AddSavingsEntryPopup::reset() {
        m_RecordName.clear();
        m_Date = getCurrentDate();
        m_Deposit = 0.0;
        m_ActualState = 0.0;
    }

    // -----------------------------------------------------------------------------------------------------------------

    AddExpenseEntryPopup::AddExpenseEntryPopup(App::AppData& appData, Store& store):
    EntryPopup(appData, store, "Add monthly expense entry", FINMGR_POPUP_ADD_EXPENSE_ENTRY) {

    }

    void AddExpenseEntryPopup::open(const std::string& expenseName, const std::optional<YearMonth> month) {
        reset();
        if (month) {
            m_Date = month->firstDay();
        }
        selectExpense(expenseName);
        m_AppData.openPopup(getID());
    }

    void AddExpenseEntryPopup::selectExpense(const std::string& name) {
        m_ExpenseName = name;
        if (!m_Store.data.monthlyExpensesManager.hasMonthlyExpense(name)) {
            return;
        }
        const MonthlyExpense& expense = m_Store.data.monthlyExpensesManager.getMonthlyExpense(name);
        m_Date.day = std::clamp(expense.defaultTransactionDay, 1, daysInMonth(m_Date.month, m_Date.year));
        m_Amount = expense.defaultTransactionAmount;
    }

    void AddExpenseEntryPopup::drawContent() {
        formWidth();
        auto& expenses = m_Store.data.monthlyExpensesManager;
        if (drawSourceSelector("Expense", sortedNames(expenses.getMonthlyExpenses()), m_ExpenseName)) {
            selectExpense(m_ExpenseName);
        }
        Ui::formLabel("Date");
        Ui::dateInput("##Date", m_Date);
        Ui::formLabel("Amount");
        ImGui::InputDouble("##Amount", &m_Amount, 1.0, 10.0, "%.2f");

        const bool valid = expenses.hasMonthlyExpense(m_ExpenseName);
        if (valid && hasEntryInMonth(expenses.getMonthlyExpense(m_ExpenseName).getEntries(), YearMonth::of(m_Date))) {
            drawWarning("There already is an entry in " + Ui::monthYear(m_Date.month, m_Date.year) + ", the table only shows the first one");
        }
        ImGui::Spacing();
        drawConfirmationButtons("Add entry", "Cancel", valid);
    }

    void AddExpenseEntryPopup::onConfirm() {
        m_Store.data.monthlyExpensesManager.getMonthlyExpense(m_ExpenseName).addEntry(m_Date, m_Amount);
    }

    void AddExpenseEntryPopup::reset() {
        m_ExpenseName.clear();
        m_Date = getCurrentDate();
        m_Amount = 0.0;
    }

}
