#include "CreatePopups.hpp"

#include "../Theme.hpp"
#include "../Ui.hpp"

#include <VelyraAppFramework/AppData.hpp>

namespace FinMgr {

    namespace {

        constexpr float FORM_WIDTH = 400.0f;

        void formWidth() {
            ImGui::Dummy(ImVec2(FORM_WIDTH * ImGui::GetStyle().FontScaleDpi, 0.0f));
        }

        std::string trimmed(const std::string& text) {
            const auto first = text.find_first_not_of(" \t");
            if (first == std::string::npos) {
                return {};
            }
            const auto last = text.find_last_not_of(" \t");
            return text.substr(first, last - first + 1);
        }

    }

    CreatePopup::CreatePopup(App::AppData& appData, Store& store, const std::string& name, const App::Widgets::PopupID id):
    Popup(appData, name, id),
    m_Store(store) {

    }

    void CreatePopup::open() {
        reset();
        m_AppData.openPopup(getID());
    }

    bool CreatePopup::drawNameInput(const char* hint, const bool exists) {
        Ui::formLabel("Name");
        if (ImGui::IsWindowAppearing()) {
            ImGui::SetKeyboardFocusHere();
        }
        ImGui::InputTextWithHint("##Name", hint, &m_Name);
        if (exists) {
            ImGui::TextColored(Theme::Warning, "This name is already used");
        }
        return !trimmed(m_Name).empty() && !exists;
    }

    // -----------------------------------------------------------------------------------------------------------------

    CreateIncomePopup::CreateIncomePopup(App::AppData& appData, Store& store):
    CreatePopup(appData, store, "New income source", FINMGR_POPUP_CREATE_INCOME) {

    }

    void CreateIncomePopup::drawContent() {
        formWidth();
        const bool valid = drawNameInput("e.g. Salary", m_Store.data.wageManager.hasWage(trimmed(m_Name)));
        ImGui::Spacing();
        drawConfirmationButtons("Create", "Cancel", valid);
    }

    void CreateIncomePopup::onConfirm() {
        const std::string name = trimmed(m_Name);
        m_Store.data.wageManager.createWage(name);
        m_Store.ui.selectedWage = name;
    }

    void CreateIncomePopup::reset() {
        m_Name.clear();
    }

    // -----------------------------------------------------------------------------------------------------------------

    CreateSavingsRecordPopup::CreateSavingsRecordPopup(App::AppData& appData, Store& store):
    CreatePopup(appData, store, "New savings account", FINMGR_POPUP_CREATE_SAVINGS_RECORD) {

    }

    void CreateSavingsRecordPopup::drawContent() {
        formWidth();
        const bool valid = drawNameInput("e.g. Savings account", m_Store.data.savingsManager.hasSavingsRecord(trimmed(m_Name)));
        Ui::formLabel("Initial balance");
        ImGui::InputDouble("##Initial", &m_InitialAmount, 10.0, 100.0, "%.2f");
        Ui::formLabel("Monthly deposit");
        ImGui::InputDouble("##Deposit", &m_DefaultDeposit, 10.0, 100.0, "%.2f");
        ImGui::SameLine();
        Ui::helpMarker("The amount you plan to deposit every month, it is filled in when adding an entry.");
        ImGui::Spacing();
        drawConfirmationButtons("Create", "Cancel", valid);
    }

    void CreateSavingsRecordPopup::onConfirm() {
        const std::string name = trimmed(m_Name);
        m_Store.data.savingsManager.createSavingsRecord(name, m_InitialAmount, m_DefaultDeposit);
        m_Store.ui.selectedSavings = name;
    }

    void CreateSavingsRecordPopup::reset() {
        m_Name.clear();
        m_InitialAmount = 0.0;
        m_DefaultDeposit = 0.0;
    }

    // -----------------------------------------------------------------------------------------------------------------

    CreateMonthlyExpensePopup::CreateMonthlyExpensePopup(App::AppData& appData, Store& store):
    CreatePopup(appData, store, "New monthly expense", FINMGR_POPUP_CREATE_MONTHLY_EXPENSE) {

    }

    void CreateMonthlyExpensePopup::drawContent() {
        formWidth();
        const bool nameValid = drawNameInput("e.g. Rent", m_Store.data.monthlyExpensesManager.hasMonthlyExpense(trimmed(m_Name)));
        Ui::formLabel("Amount");
        ImGui::InputDouble("##Amount", &m_Amount, 1.0, 10.0, "%.2f");
        Ui::formLabel("Day of the month");
        ImGui::InputInt("##Day", &m_Day);
        m_Day = std::clamp(m_Day, 1, 28);
        ImGui::SameLine();
        Ui::helpMarker("Every month without an entry gets one on this day with the amount above when the app starts. "
                       "Limited to 28 so it exists in every month.");

        const Date today = getCurrentDate();
        const std::string startLabel = "Add an entry for " + Ui::monthYear(today.month, today.year);
        ImGui::Checkbox(startLabel.c_str(), &m_StartThisMonth);
        if (!m_StartThisMonth) {
            ImGui::TextDisabled("The expense shows up in the table once it has an entry.");
        }
        ImGui::Spacing();
        drawConfirmationButtons("Create", "Cancel", nameValid);
    }

    void CreateMonthlyExpensePopup::onConfirm() {
        const std::string name = trimmed(m_Name);
        auto& manager = m_Store.data.monthlyExpensesManager;
        manager.createMonthlyExpense(name, m_Day, m_Amount);
        if (m_StartThisMonth) {
            const Date today = getCurrentDate();
            manager.getMonthlyExpense(name).addEntry(Date(m_Day, today.month, today.year), m_Amount);
        }
    }

    void CreateMonthlyExpensePopup::reset() {
        m_Name.clear();
        m_Day = 1;
        m_Amount = 0.0;
        m_StartThisMonth = true;
    }

}
