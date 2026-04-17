#include "SavingsTrendPanel.hpp"

#include <VelyraAppFramework/Styles.hpp>

namespace FinMgr {

    SavingsTrendPanel::SavingsTrendPanel(App::AppData& appData, SavingsRecord& record):
    Panel(appData, "Savings Trend") {
        createSavingsData(record);
    }

    void SavingsTrendPanel::drawContent() {
        const std::string plot_id = "Savings Trend##" + getName();
        const ImVec2 contentRegion = ImGui::GetContentRegionAvail();
        if (ImPlot::BeginPlot(plot_id.c_str(), contentRegion)) {
            ImPlot::SetupAxes("Time", "Amount");
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

            ImPlot::PlotLine("Expected Savings", m_Times.data(), m_ExpectedSavings.data(), m_Times.size());
            ImPlot::PlotLine("Actual Savings", m_Times.data(), m_ActualSavings.data(), m_Times.size());

            if (ImPlot::IsPlotHovered()) {
                const ImPlotPoint p = ImPlot::GetPlotMousePos();

                // Find nearest X (time)
                auto it = std::ranges::lower_bound(m_Times, p.x);

                if (it != m_Times.end()) {
                    const Size idx = std::distance(m_Times.begin(), it);

                    const double t = m_Times[idx];
                    const double expected = m_ExpectedSavings[idx];
                    const double actual = m_ActualSavings[idx];

                    ImGui::BeginTooltip();
                    ImGui::Text("Time : %s", Utils::toString(TimePoint{Duration{(t)}}).c_str());
                    ImGui::Text("Expected: %.2f", expected);
                    ImGui::Text("Actual: %.2f", actual);
                    ImGui::EndTooltip();
                }
            }
            ImPlot::EndPlot();
        }
    }

    void SavingsTrendPanel::createSavingsData(const SavingsRecord &record) {
        const auto& entries = record.getEntries();
        m_Times.reserve(entries.size());
        m_ExpectedSavings.reserve(entries.size());
        m_ActualSavings.reserve(entries.size());

        double cumulativeExpected = record.initialAmount;
        for (const auto&[date, expectedTransactionAmount, actualState] : entries) {
            m_Times.push_back(date.toUnixTimestamp());
            cumulativeExpected += expectedTransactionAmount;
            m_ExpectedSavings.push_back(cumulativeExpected);
            m_ActualSavings.push_back(actualState);
        }
    }
}
