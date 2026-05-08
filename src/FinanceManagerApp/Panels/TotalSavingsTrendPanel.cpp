#include "TotalSavingsTrendPanel.hpp"

#include "../Utils.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {
    TotalSavingsTrendPanel::TotalSavingsTrendPanel(App::AppData &appData, const Store& store):
    Panel(appData, "Total Savings Trend"),
    m_Trend(store.savingsManager.getTotalSavingsTrend())
    {
        createTrendData();
    }

    void TotalSavingsTrendPanel::drawContent() {
        drawDetails();
        drawGraph();
    }

    void TotalSavingsTrendPanel::drawDetails() {
        ImGui::Text("Initial Amount: %.2f", m_Trend.actualAmounts.empty() ? 0.0 : m_Trend.actualAmounts.front());
        ImGui::Text("Final Amount: %.2f", m_Trend.actualAmounts.empty() ? 0.0 : m_Trend.actualAmounts.back());
        ImGui::Text("Trend Line Slope (per month): %.2f", m_AvgMonthlyChange);
    }

    void TotalSavingsTrendPanel::drawGraph() {
        const std::string plot_id = "Total Savings Trend##" + getName();
        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        if (ImPlot::BeginPlot(plot_id.c_str(), contentRegion)) {
            ImPlot::SetupAxes("Time", "Amount");
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

            ImPlot::PlotLine("Expected Savings", m_Trend.dates.data(), m_Trend.expectedAmounts.data(), m_Trend.dates.size());
            ImPlot::PlotLine("Actual Savings", m_Trend.dates.data(), m_Trend.actualAmounts.data(), m_Trend.dates.size());
            ImPlot::PlotLine("Trend Line", m_TimeRange.data(), m_TrendPoints.data(), 2);

            if (ImPlot::IsPlotHovered()) {
                const ImPlotPoint p = ImPlot::GetPlotMousePos();

                // Find nearest X (time)
                auto it = std::ranges::lower_bound(m_Trend.dates, p.x);

                if (it != m_Trend.dates.end()) {
                    const Size idx = std::distance(m_Trend.dates.begin(), it);

                    const double t = m_Trend.dates[idx];
                    const double amount = m_Trend.actualAmounts[idx];

                    ImGui::BeginTooltip();
                    ImGui::Text("Time : %s", Utils::toString(TimePoint{Duration{(t)}}).c_str());
                    ImGui::Text("Total Savings: %.2f", amount);
                    ImGui::EndTooltip();
                }
            }
            ImPlot::EndPlot();
        }
    }

    void TotalSavingsTrendPanel::createTrendData() {
        const OLSLinearRegression ols = performOLSLinearRegression(m_Trend.dates, m_Trend.actualAmounts);
        m_AvgMonthlyChange = ols.slope * 30.0 * 24.0 * 3600.0; // Convert from per second to per month;
        const double intercept = ols.intercept;
        const double x_min = m_Trend.dates.empty() ? 0 : m_Trend.dates.front();
        const double x_max = m_Trend.dates.empty() ? 1 : m_Trend.dates.back();
        const double trend_start = ols.slope * x_min + intercept;
        const double trend_end = ols.slope * x_max + intercept;

        m_TimeRange = {x_min, x_max};
        m_TrendPoints = {trend_start, trend_end};
    }
}
