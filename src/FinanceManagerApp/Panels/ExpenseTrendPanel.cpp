#include "ExpenseTrendPanel.hpp"

#include "../Utils.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {
    ExpenseTrendPanel::ExpenseTrendPanel(App::AppData &appData, Store &store):
    Panel(appData, "Total Expense Trend"),
    m_Store(store){
        createGraphData(store);
    }

    void ExpenseTrendPanel::drawContent() {
        drawDetails();
        drawGraph();
    }

    void ExpenseTrendPanel::drawDetails() {
        const auto max_it = std::ranges::max_element(m_Values);
        Size maxIdx = std::distance(m_Values.begin(), max_it);
        const auto min_it = std::ranges::min_element(m_Values);
        Size minIdx = std::distance(m_Values.begin(), min_it);

        const TimePoint maxTime = TimePoint(Duration{m_Times[maxIdx]});
        const TimePoint minTime = TimePoint(Duration{m_Times[minIdx]});
        ImGui::Text("Most Expensive month: %s with %.2f", Utils::toString(maxTime).c_str(), m_Values[maxIdx]);
        ImGui::Text("Least Expensive month: %s with %.2f", Utils::toString(minTime).c_str(), m_Values[minIdx]);
    }

    void ExpenseTrendPanel::drawGraph() {
        const std::string plot_id = "Expense Trend##" + getName();
        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        if (ImPlot::BeginPlot(plot_id.c_str(), contentRegion)) {
            ImPlot::SetupAxes("Time", "Amount");
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

            ImPlot::PlotLine("Monthly Expense Cost", m_Times.data(), m_Values.data(), m_Times.size());

            if (ImPlot::IsPlotHovered()) {
                const ImPlotPoint p = ImPlot::GetPlotMousePos();

                // Find nearest X (time)
                auto it = std::ranges::lower_bound(m_Times, p.x);

                if (it != m_Times.end()) {
                    const Size idx = std::distance(m_Times.begin(), it);

                    const double t = m_Times[idx];
                    const double monthlyCost = m_Values[idx];

                    ImGui::BeginTooltip();
                    ImGui::Text("Time : %s", Utils::toString(TimePoint{Duration{(t)}}).c_str());
                    ImGui::Text("Montly Cost: %.2f", monthlyCost);
                    ImGui::EndTooltip();
                }
            }
            ImPlot::EndPlot();
        }
    }

    void ExpenseTrendPanel::createGraphData(Store &store) {
        // Find the earliest date across all expenses
        Date earliestDate = getCurrentDate();
        for (auto &expense: m_Store.monthlyExpensesManager.getMonthlyExpenses() | std::views::values) {
            auto& entries = expense.getEntries();
            if (!entries.empty()) {
                if (entries.front().date < earliestDate) {
                    earliestDate = entries.front().date;
                }
            }
        }

        // reset earliest date.day to 1 to take all records into account
        earliestDate.day = 1;
        const Date now = getCurrentDate();
        while (earliestDate <= now) {
            double currentTotal = 0.0;
            for (const auto& [_, expense]: m_Store.monthlyExpensesManager.getMonthlyExpenses()) {
                const auto& entries = expense.getEntries();
                for (const auto& entry: entries) {
                    if (entry.date.year == earliestDate.year && entry.date.month == earliestDate.month) {
                        currentTotal += entry.amount;
                    }
                }
            }
            m_Values.push_back(currentTotal);
            m_Times.push_back(earliestDate.toUnixTimestamp());
            earliestDate.monthIncrement();
        }
    }
}
