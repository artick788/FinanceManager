#pragma once

#include <VelyraCore/ImGui/ImGui.hpp>

namespace FinMgr::Theme {

    /**
     * @brief Applies the colors and sizes of the app, call once after the ImGui context is created. Sizes are scaled by
     *        the DPI scale the framework applied, so the app looks the same on every monitor.
     */
    void apply();

    // Semantic colors, use these instead of raw colors so the app stays consistent
    constexpr auto Accent        = ImVec4(0.33f, 0.58f, 1.00f, 1.00f);
    constexpr auto Positive      = ImVec4(0.30f, 0.78f, 0.47f, 1.00f);
    constexpr auto Negative      = ImVec4(0.96f, 0.36f, 0.34f, 1.00f);
    constexpr auto Warning       = ImVec4(0.93f, 0.68f, 0.25f, 1.00f);
    constexpr auto Muted         = ImVec4(0.56f, 0.60f, 0.68f, 1.00f);
    constexpr auto CardBg        = ImVec4(0.12f, 0.14f, 0.18f, 1.00f);

    // Plot colors
    constexpr auto Income        = ImVec4(0.30f, 0.78f, 0.47f, 1.00f);
    constexpr auto Expenses      = ImVec4(0.96f, 0.42f, 0.38f, 1.00f);
    constexpr auto Savings       = ImVec4(0.33f, 0.58f, 1.00f, 1.00f);
    constexpr auto Expected      = ImVec4(0.75f, 0.62f, 0.98f, 1.00f);
    constexpr auto Trend         = ImVec4(0.93f, 0.68f, 0.25f, 1.00f);

    // Table rows
    constexpr auto YearRowBg     = ImVec4(1.00f, 0.86f, 0.45f, 0.14f); // Yearly totals, the highlight of the old app
    constexpr auto TotalRowBg    = ImVec4(0.30f, 0.78f, 0.47f, 0.20f); // All time totals

}
