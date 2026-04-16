#include "AddMonthlyExpensePopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

#include "../Widgets/DateInput.hpp"

namespace FinMgr {

    AddMonthlyExpensePopup::AddMonthlyExpensePopup(Store& store) :
    Popup("Add Monthly Expense"),
    m_Store(store) {
    }

    void AddMonthlyExpensePopup::drawContent() {
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
                    m_InputDate.day = m_Store.monthlyExpensesManager.getMonthlyExpense(name).defaultTransactionDay;
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
