#include "MonthlyExpenseOverview.hpp"

#include "Utils.hpp"

#include "Popups/AddMonthlyExpensePopup.hpp"
#include "Panels/ExpenseTrendPanel.hpp"

#include <VelyraAppFramework/Styles.hpp>
#include <ranges>

namespace FinMgr {

    MonthlyExpenseOverview::MonthlyExpenseOverview(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store) {

    }

    void MonthlyExpenseOverview::drawStatistics() const {
        auto& expenses = m_Store.monthlyExpensesManager.getMonthlyExpenses();
        if (expenses.empty()) {
            return;
        }

        const Date now = getCurrentDate();
        double totalMonthlyExpense = 0.0;
        
        for (auto& [name, expense] : expenses) {
            auto& entries = expense.getEntries();
            if (!entries.empty()) {
                // Get the most recent entry for each expense
                for (auto &[date, amount] : std::views::reverse(entries)) {
                    if (date <= now) {
                        totalMonthlyExpense += amount;
                        break;
                    }
                }
            }
            else if (expense.defaultTransactionAmount > 0.0) {
                totalMonthlyExpense += expense.defaultTransactionAmount;
            }
        }

        ImGui::TextWrapped("Total Monthly Expenses: %.2f", totalMonthlyExpense);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        if (ImGui::Button("Show total expense evolution")) {
            m_AppData.addPanel(createSP<ExpenseTrendPanel>(m_AppData, m_Store));
        }
    }

    void MonthlyExpenseOverview::drawDetail() {
        if (ImGui::Button("Add Monthly Expense")) {
            m_Store.addMonthlyExpensePopup->setOpen(true);
        }
        drawTable();
    }

    void MonthlyExpenseOverview::drawTable() {
        auto& expenses = m_Store.monthlyExpensesManager.getMonthlyExpenses();
        if (expenses.empty()) {
            ImGui::Text("No monthly expenses added yet.");
            return;
        }

        // Find the earliest date across all expenses
        Date earliestDate = getCurrentDate();
        bool hasAnyEntry = false;
        
        for (auto& [name, expense] : expenses) {
            auto& entries = expense.getEntries();
            if (!entries.empty()) {
                if (!hasAnyEntry || entries.front().date < earliestDate) {
                    earliestDate = entries.front().date;
                    hasAnyEntry = true;
                }
            }
        }

        if (!hasAnyEntry) {
            ImGui::Text("No expense entries added yet.");
            return;
        }

        // Set up table with columns: Year, Month, and one column per expense, plus Total
        const Size columnCount = 2 + expenses.size() + 1; // Year, Month, expenses, Total
        
        if (ImGui::BeginTable("MonthlyExpensesTable", columnCount, ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Year");
            ImGui::TableSetupColumn("Month");
            
            // Set up column for each expense
            std::vector<std::string> expenseNames;
            for (auto& [name, expense] : expenses) {
                expenseNames.push_back(name);
                ImGui::TableSetupColumn(name.c_str());
            }
            ImGui::TableSetupColumn("Total");
            ImGui::TableHeadersRow();

            // Iterate through months from earliest to now
            Date currentDate = earliestDate;
            currentDate.day = 1; // Start from first day of the month
            const Date now = getCurrentDate();
            
            int currentYear = currentDate.year;
            bool drawYear = true;
            std::unordered_map<std::string, double> yearlyTotals;
            std::unordered_map<std::string, double> totalTotals;
            
            // Initialize maps
            for (const auto& expenseName : expenseNames) {
                yearlyTotals[expenseName] = 0.0;
                totalTotals[expenseName] = 0.0;
            }
            
            while (currentDate <= now) {
                ImGui::TableNextRow();
                
                // Check if we moved to a new year
                if (currentDate.year > currentYear) {
                    // Add yearly totals to total totals
                    for (const auto& expenseName : expenseNames) {
                        totalTotals[expenseName] += yearlyTotals[expenseName];
                    }
                    
                    // Draw yearly total row
                    drawTotalRow(expenseNames, yearlyTotals, App::Styles::ColorHighLight);
                    tableEmptyRow(columnCount);
                    
                    ImGui::TableNextRow();
                    currentYear = currentDate.year;
                    
                    // Clear yearly totals
                    for (const auto& expenseName : expenseNames) {
                        yearlyTotals[expenseName] = 0.0;
                    }
                    
                    drawYear = true;
                }
                
                // Draw Year column
                ImGui::TableSetColumnIndex(0);
                if (drawYear) {
                    ImGui::Text("%d", currentDate.year);
                    drawYear = false;
                }
                else {
                    ImGui::TextUnformatted("");
                }
                
                // Draw Month column
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", getMonthStr(currentDate.month));
                
                // Draw each expense amount for this month
                Size colIndex = 2;
                for (const auto& expenseName : expenseNames) {
                    ImGui::TableSetColumnIndex(colIndex++);
                    
                    auto& expense = expenses[expenseName];
                    auto& entries = expense.getEntries();
                    
                    // Find entry for this month
                    bool foundEntry = false;
                    for (auto& entry : entries) {
                        if (entry.date.year == currentDate.year && entry.date.month == currentDate.month) {
                            const std::string editLabel = "##" + expenseName + std::to_string(currentDate.year) + std::to_string(currentDate.month);
                            ImGui::InputDouble(editLabel.c_str(), &entry.amount, 0.0, 0.0, "%.2f");
                            yearlyTotals[expenseName] += entry.amount;
                            foundEntry = true;
                            break;
                        }
                    }
                    
                    if (!foundEntry) {
                        // Check if this expense had started by this month
                        if (!entries.empty() && currentDate >= Date{1, entries.front().date.month, entries.front().date.year}) {
                            ImGui::Text("N/A");
                        }
                        else {
                            ImGui::TextUnformatted("");
                        }
                    }
                }
                
                // Draw Total column
                ImGui::TableSetColumnIndex(colIndex);
                double monthTotal = 0.0;
                for (const auto& expenseName : expenseNames) {
                    auto& expense = expenses[expenseName];
                    auto& entries = expense.getEntries();
                    for (auto& entry : entries) {
                        if (entry.date.year == currentDate.year && entry.date.month == currentDate.month) {
                            monthTotal += entry.amount;
                            break;
                        }
                    }
                }
                if (monthTotal > 0.0) {
                    ImGui::Text("%.2f", monthTotal);
                }
                else {
                    ImGui::TextUnformatted("");
                }
                
                // Move to next month
                currentDate.monthIncrement();
                currentDate.day = 1;
            }
            
            // Add final year's totals to total totals
            for (const auto& expenseName : expenseNames) {
                totalTotals[expenseName] += yearlyTotals[expenseName];
            }
            
            // Draw final yearly total
            drawTotalRow(expenseNames, yearlyTotals, App::Styles::ColorHighLight);
            
            // Draw overall total
            drawTotalRow(expenseNames, totalTotals, App::Styles::ColorGreen);
            
            ImGui::EndTable();
        }
    }

    void MonthlyExpenseOverview::drawTotalRow(const std::vector<std::string>& expenseNames, const std::unordered_map<std::string, double>& totals, const ImVec4& color) {
        ImGui::TableNextRow();
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(color));
        ImGui::TableSetColumnIndex(0);
        ImGui::Text("Total:");
        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted("");
        
        // Draw individual expense totals
        double grandTotal = 0.0;
        for (Size i = 0; i < expenseNames.size(); ++i) {
            ImGui::TableSetColumnIndex(2 + i);
            const double expenseTotal = totals.at(expenseNames[i]);
            if (expenseTotal > 0.0) {
                ImGui::Text("%.2f", expenseTotal);
                grandTotal += expenseTotal;
            }
            else {
                ImGui::TextUnformatted("");
            }
        }
        
        // Draw grand total
        ImGui::TableSetColumnIndex(2 + expenseNames.size());
        ImGui::Text("%.2f", grandTotal);
    }
}
