#pragma once

#include <VelyraAppFramework/Themes/Dusk.hpp>
#include <VelyraCore/ImGui/ImGui.hpp>

/**
 * @brief The app uses the Dusk theme of the framework, apply it with Dusk::apply(context). This file adds the colors
 *        that only make sense for the finances on top of the colors of the theme.
 */
namespace FinMgr::Theme {

    namespace Dusk = Velyra::App::Themes::Dusk;

    // Semantic colors of the theme, use these instead of raw colors so the app stays consistent
    constexpr auto Accent        = Dusk::Accent;
    constexpr auto Positive      = Dusk::Positive;
    constexpr auto Negative      = Dusk::Negative;
    constexpr auto Warning       = Dusk::Warning;
    constexpr auto Muted         = Dusk::Muted;
    constexpr auto CardBg        = Dusk::CardBg;

    // Plot colors
    constexpr auto Income        = Dusk::Positive;
    constexpr auto Expenses      = ImVec4(0.96f, 0.42f, 0.38f, 1.00f);
    constexpr auto Savings       = Dusk::Accent;
    constexpr auto Expected      = ImVec4(0.75f, 0.62f, 0.98f, 1.00f);
    constexpr auto Trend         = Dusk::Warning;

    // Table rows
    constexpr auto YearRowBg     = ImVec4(1.00f, 0.86f, 0.45f, 0.14f); // Yearly totals, the highlight of the old app
    constexpr auto TotalRowBg    = ImVec4(0.30f, 0.79f, 0.50f, 0.20f); // All time totals

}
