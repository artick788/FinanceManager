#include "CreateMonthlyExpensePopup.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    CreateMonthlyExpensePopup::CreateMonthlyExpensePopup(Store& store):
    Popup("Add Monthly Expense"),
    m_Store(store) {

    }

    void CreateMonthlyExpensePopup::drawContent() {
        ImGui::InputTextWithHint("##MonthlyExpenseName", "Enter monthly expense name", &m_MonthlyExpenseName);
        ImGui::InputInt("Default Transaction Day", &m_InputDefaultTransactionDay, 1, 0);
        ImGui::InputDouble("Default Transaction Amount", &m_InputDefaultTransactionAmount, 0.01, 0.0, "%.2f");

        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button("Add", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            if (!m_MonthlyExpenseName.empty() &&
                !m_Store.monthlyExpensesManager.hasMonthlyExpense(m_MonthlyExpenseName) &&
                m_InputDefaultTransactionDay >= 1 && m_InputDefaultTransactionDay <= 28) {
                m_Store.monthlyExpensesManager.createMonthlyExpense(m_MonthlyExpenseName, m_InputDefaultTransactionDay, m_InputDefaultTransactionAmount);
                reset();
                setOpen(false);
            }
        }
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            reset();
            setOpen(false);
        }
        ImGui::PopStyleColor();
    }

    void CreateMonthlyExpensePopup::reset() {
        m_MonthlyExpenseName.clear();
        m_InputDefaultTransactionDay = 1;
        m_InputDefaultTransactionAmount = 0.0;
    }
}