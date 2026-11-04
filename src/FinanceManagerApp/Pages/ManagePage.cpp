#include "ManagePage.hpp"

#include "../Popups/ConfirmPopup.hpp"
#include "../Popups/CreatePopups.hpp"
#include "../Theme.hpp"
#include "../Ui.hpp"

#include <ranges>

namespace FinMgr {

    namespace {

        template<typename MAP>
        std::vector<std::string> sortedNames(const MAP& map) {
            std::vector<std::string> names;
            for (const auto& name: map | std::views::keys) {
                names.push_back(name);
            }
            std::ranges::sort(names);
            return names;
        }

        void header(const char* title, const char* buttonLabel, const std::function<void()>& onClick) {
            Ui::panelTitle(title);
            Ui::sameLineRight(Ui::buttonWidth(buttonLabel));
            if (ImGui::Button(buttonLabel)) {
                onClick();
            }
        }

        template<typename ENTRIES>
        std::string dateRange(const ENTRIES& entries) {
            if (entries.empty()) {
                return "No entries";
            }
            return Ui::monthYear(entries.front().date.month, entries.front().date.year) + " - " +
                   Ui::monthYear(entries.back().date.month, entries.back().date.year);
        }

        bool deleteButton() {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Theme::Negative.x, Theme::Negative.y, Theme::Negative.z, 0.25f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(Theme::Negative.x, Theme::Negative.y, Theme::Negative.z, 0.7f));
            const bool clicked = ImGui::SmallButton("Delete");
            ImGui::PopStyleColor(2);
            return clicked;
        }

        constexpr ImGuiTableFlags TABLE_FLAGS = ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuterH |
            ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;

    }

    ManagePage::ManagePage(Store& store):
    m_Store(store) {

    }

    SP<App::Node> ManagePage::buildLayout() {
        using namespace App;
        return horizontalSplit({.name = "ManagePage"},
            createPanel({.name = "Manage Income", .drawFunction = bindDraw(this, &ManagePage::drawIncome), .sizeRatio = 0.28f}),
            createPanel({.name = "Manage Savings", .drawFunction = bindDraw(this, &ManagePage::drawSavings), .sizeRatio = 0.36f}),
            createPanel({.name = "Manage Expenses", .drawFunction = bindDraw(this, &ManagePage::drawExpenses), .sizeRatio = 0.36f})
        );
    }

    void ManagePage::askDelete(const std::string& what, const std::string& name, const Size entryCount, std::function<void()> action) {
        std::string message = "Delete " + what + " '" + name + "'";
        if (entryCount > 0) {
            message += " and its " + std::to_string(entryCount) + (entryCount == 1 ? " entry" : " entries");
        }
        m_Store.popups.confirm->ask(message + "?", "Delete", std::move(action));
    }

    void ManagePage::drawIncome() {
        header("Income sources", "+ New", [this] { m_Store.popups.createIncome->open(); });
        auto& wages = m_Store.data.wageManager;
        if (wages.getWages().empty()) {
            ImGui::TextDisabled("No income sources yet");
            return;
        }
        if (!ImGui::BeginTable("##Income", 3, TABLE_FLAGS)) {
            return;
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Paychecks", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        for (const auto& name: sortedNames(wages.getWages())) {
            const Wage& wage = wages.getWage(name);
            ImGui::TableNextRow();
            ImGui::PushID(name.c_str());
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            if (ImGui::Selectable(name.c_str())) {
                m_Store.ui.selectedWage = name;
                m_Store.ui.setPage(Page::Income);
            }
            ImGui::SetItemTooltip("Open the paychecks of %s", name.c_str());
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%zu", wage.getEntries().size());
            ImGui::SameLine();
            ImGui::TextDisabled("%s", dateRange(wage.getEntries()).c_str());
            ImGui::TableSetColumnIndex(2);
            if (deleteButton()) {
                askDelete("income source", name, wage.getEntries().size(), [this, name] { m_Store.data.wageManager.destroyWage(name); });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    void ManagePage::drawSavings() {
        header("Savings accounts", "+ New", [this] { m_Store.popups.createSavingsRecord->open(); });
        auto& savings = m_Store.data.savingsManager;
        if (savings.getSavingsRecords().empty()) {
            ImGui::TextDisabled("No savings accounts yet");
            return;
        }
        if (!ImGui::BeginTable("##Savings", 4, TABLE_FLAGS)) {
            return;
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Initial balance", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Monthly deposit", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        for (const auto& name: sortedNames(savings.getSavingsRecords())) {
            SavingsRecord& record = savings.getSavingsRecord(name);
            ImGui::TableNextRow();
            ImGui::PushID(name.c_str());
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            if (ImGui::Selectable(name.c_str())) {
                m_Store.ui.selectedSavings = name;
                m_Store.ui.setPage(Page::Savings);
            }
            ImGui::SetItemTooltip("%s\n%zu entries", dateRange(record.getEntries()).c_str(), record.getEntries().size());
            ImGui::TableSetColumnIndex(1);
            Ui::amountInput("##Initial", &record.initialAmount);
            ImGui::TableSetColumnIndex(2);
            Ui::amountInput("##Deposit", &record.defaultTransactionAmount);
            ImGui::TableSetColumnIndex(3);
            if (deleteButton()) {
                askDelete("savings account", name, record.getEntries().size(), [this, name] { m_Store.data.savingsManager.destroySavingsRecord(name); });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    void ManagePage::drawExpenses() {
        header("Monthly expenses", "+ New", [this] { m_Store.popups.createMonthlyExpense->open(); });
        auto& expenses = m_Store.data.monthlyExpensesManager;
        if (expenses.getMonthlyExpenses().empty()) {
            ImGui::TextDisabled("No monthly expenses yet");
            return;
        }
        if (!ImGui::BeginTable("##Expenses", 4, TABLE_FLAGS)) {
            return;
        }
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Default amount", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Day", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("##Actions", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        for (const auto& name: sortedNames(expenses.getMonthlyExpenses())) {
            MonthlyExpense& expense = expenses.getMonthlyExpense(name);
            ImGui::TableNextRow();
            ImGui::PushID(name.c_str());
            ImGui::TableSetColumnIndex(0);
            ImGui::AlignTextToFramePadding();
            if (ImGui::Selectable(name.c_str())) {
                m_Store.ui.setPage(Page::Expenses);
            }
            ImGui::SetItemTooltip("%s\n%zu entries", dateRange(expense.getEntries()).c_str(), expense.getEntries().size());
            ImGui::TableSetColumnIndex(1);
            Ui::amountInput("##Amount", &expense.defaultTransactionAmount);
            ImGui::TableSetColumnIndex(2);
            ImGui::SetNextItemWidth(-FLT_MIN);
            if (ImGui::InputInt("##Day", &expense.defaultTransactionDay)) {
                expense.defaultTransactionDay = std::clamp(expense.defaultTransactionDay, 1, 28);
            }
            ImGui::TableSetColumnIndex(3);
            if (deleteButton()) {
                askDelete("monthly expense", name, expense.getEntries().size(), [this, name] { m_Store.data.monthlyExpensesManager.destroyMonthlyExpense(name); });
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

}
