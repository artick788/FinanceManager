#include "TotalsLayout.hpp"

#include "Utils.hpp"

namespace FinMgr {

    TotalsLayout::TotalsLayout(App::AppData& appData, Store& store) :
    m_AppData(appData),
    m_Store(store) {

    }

    SP<App::Node> TotalsLayout::createLayout() const {
        using namespace Velyra::App;

        return App::createLayout(
            horizontalSplit(
                verticalSplit(
                    createPanel({.name = "Main", .sizeRatio = 0.4f}),
                    createPanel({.name = "Options", .sizeRatio = 0.6f})
                ),
                createPanel({.name = "Statistic", .sizeRatio = 0.7f})
            )
        );
    }

    void TotalsLayout::draw() {
        m_AppData.layoutEngine.beginPanel("Main");
        drawMain();
        m_AppData.layoutEngine.endPanel("Main");

        m_AppData.layoutEngine.beginPanel("Options");
        drawOptions();
        m_AppData.layoutEngine.endPanel("Options");

        m_AppData.layoutEngine.beginPanel("Statistic");
        drawStatistic();
        m_AppData.layoutEngine.endPanel("Statistic");
    }

    void TotalsLayout::drawMain() {
        if (ImGui::BeginListBox("##Items", ImVec2(-FLT_MIN, -FLT_MIN))) {
            for (int i = 0; i < s_ItemTypes.size(); ++i) {
                const bool is_selected = m_SelectedItemType == i;
                if (ImGui::Selectable(s_ItemTypes[i].c_str(), is_selected)) {
                    m_SelectedItemType = i;
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndListBox();
        }
    }

    void TotalsLayout::drawOptions() {
        switch (m_SelectedItemType) {
            case 0:
                ImGui::Text("Options for Income");
                break;
            case 1:
                drawOptionsSavings();
                break;
            case 2:
                ImGui::Text("Options for Monthly Expenses");
                break;
            default:
                ImGui::Text("Select an item to see options");
        }
    }

    void TotalsLayout::drawOptionsSavings() {
        if (ImGui::Button("Show Savings Evolution")) {
            m_SelectedTotalsType = TotalSavingsEvolution;
        }
    }

    void TotalsLayout::drawStatistic() {
        switch (m_SelectedTotalsType) {
            case TotalSavingsEvolution:
                drawTotalSavingsEvolution();
                break;
            default:
                ImGui::Text("Select an option to see statistics");
        }
    }

    void TotalsLayout::drawTotalSavingsEvolution() {
        const std::string plotID = "Total Savings Evolution";
        const TotalSavingsTrend trend = m_Store.savingsManager.getTotalSavingsTrend();
        const OLSLinearRegression ols = performOLSLinearRegression(trend.dates, trend.actualAmounts);
        const double slope = ols.slope;
        const double intercept = ols.intercept;
        const double x_min = trend.dates.empty() ? 0 : trend.dates.front();
        const double x_max = trend.dates.empty() ? 1 : trend.dates.back();
        const double trend_start = slope * x_min + intercept;
        const double trend_end = slope * x_max + intercept;

        const double trendPoints[2] = {trend_start, trend_end};
        const double timeRange[2] = {x_min, x_max};

        ImGui::Text("Trend Line Slope: %.2f", slope);
        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        if (ImPlot::BeginPlot(plotID.c_str(), contentRegion)) {
            ImPlot::SetupAxes("Time", "Amount");
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

            ImPlot::PlotLine("Total Savings", trend.dates.data(), trend.actualAmounts.data(), trend.dates.size());
            ImPlot::PlotLine("Trend Line", timeRange, trendPoints, 2);
            if (ImPlot::IsPlotHovered()) {
                const ImPlotPoint p = ImPlot::GetPlotMousePos();

                // Find nearest X (time)
                auto it = std::ranges::lower_bound(trend.dates, p.x);

                if (it != trend.dates.end()) {
                    const Size idx = std::distance(trend.dates.begin(), it);

                    const double t = trend.dates[idx];
                    const double amount = trend.actualAmounts[idx];

                    ImGui::BeginTooltip();
                    ImGui::Text("Time : %s", Utils::toString(TimePoint{Duration{(t)}}).c_str());
                    ImGui::Text("Total Savings: %.2f", amount);
                    ImGui::EndTooltip();
                }
            }
            ImPlot::EndPlot();
        }
    }
}
