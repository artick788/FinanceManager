#include "AddMonthlyExpensePopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

#include "../Widgets/DateInput.hpp"
#include "../Utils.hpp"

namespace FinMgr {

    AddMonthlyExpensePopup::AddMonthlyExpensePopup(App::AppData& appData, Store& store) :
    Popup(appData, "Add Monthly Expense", FINMGR_APP_POPUP_ADD_MONTHLY_EXPENSE),
    m_Store(store) {
    }

    void AddMonthlyExpensePopup::drawContent(Core::Window &window, Core::Context &context) {
        drawMonthlyExpenseSelector();
        drawAddAEntry();
        drawButtons();
    }

    void AddMonthlyExpensePopup::reset() {
        m_SelectedExpenseName.clear();
        m_InputDate = getCurrentDate();
        m_InputAmount = 0.0;
        m_FullyFilled = false;
    }

    void AddMonthlyExpensePopup::drawMonthlyExpenseSelector() {
        if (ImGui::BeginCombo("##ExpenseSelector", m_SelectedExpenseName.empty() ? "Select Expense" : m_SelectedExpenseName.c_str())) {
            for (const auto& [name, expense] : m_Store.monthlyExpensesManager.getMonthlyExpenses()) {
                if (ImGui::Selectable(name.c_str())) {
                    m_SelectedExpenseName = name;
                    m_FullyFilled = true; // Assuming selecting an expense is the only required field for now
                    MonthlyExpense& selectedExpense = m_Store.monthlyExpensesManager.getMonthlyExpense(name);
                    m_InputDate.day = selectedExpense.defaultTransactionDay;
                    m_InputAmount = selectedExpense.defaultTransactionAmount;
                }
            }
            ImGui::EndCombo();
        }
    }

    void AddMonthlyExpensePopup::drawAddAEntry() {
        if (m_SelectedExpenseName.empty()) {
            ImGui::TextWrapped("Please select a monthly expense to add an entry.");
            return;
        }
        ImGui::InputDouble("Amount", &m_InputAmount, 0.01, 0.0, "%.2f");
        Widgets::DateInput(m_InputDate, true);
    }

    void AddMonthlyExpensePopup::drawButtons() {
        ImGui::BeginDisabled(!m_FullyFilled);
        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            m_Store.monthlyExpensesManager.getMonthlyExpense(m_SelectedExpenseName).addEntry(m_InputDate, m_InputAmount);
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
    }
}
