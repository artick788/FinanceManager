#include "Theme.hpp"

namespace FinMgr::Theme {

    namespace {

        constexpr ImVec4 rgb(const int r, const int g, const int b, const float a = 1.0f) {
            return {static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, a};
        }

        constexpr ImVec4 withAlpha(const ImVec4 color, const float alpha) {
            return {color.x, color.y, color.z, alpha};
        }

    }

    void apply() {
        ImGuiStyle& style = ImGui::GetStyle();
        // The framework scaled all sizes for the DPI of the monitor, keep that scale for our sizes
        const float scale = style.FontScaleDpi > 0.0f ? style.FontScaleDpi : 1.0f;

        style.WindowPadding     = ImVec2(14.0f * scale, 12.0f * scale);
        style.FramePadding      = ImVec2(8.0f * scale, 4.0f * scale);
        style.CellPadding       = ImVec2(8.0f * scale, 4.0f * scale);
        style.ItemSpacing       = ImVec2(8.0f * scale, 7.0f * scale);
        style.ItemInnerSpacing  = ImVec2(6.0f * scale, 4.0f * scale);
        style.ScrollbarSize     = 12.0f * scale;
        style.GrabMinSize       = 10.0f * scale;

        style.WindowRounding    = 0.0f; // Panels tile the window
        style.ChildRounding     = 6.0f * scale;
        style.FrameRounding     = 4.0f * scale;
        style.PopupRounding     = 6.0f * scale;
        style.ScrollbarRounding = 6.0f * scale;
        style.GrabRounding      = 4.0f * scale;
        style.TabRounding       = 4.0f * scale;

        style.WindowBorderSize  = 1.0f;
        style.ChildBorderSize   = 1.0f;
        style.PopupBorderSize   = 1.0f;
        style.FrameBorderSize   = 0.0f;
        style.SeparatorTextBorderSize = 1.0f;
        style.WindowMenuButtonPosition = ImGuiDir_None;

        ImVec4* colors = style.Colors;
        const ImVec4 background = rgb(21, 24, 30);
        const ImVec4 surface    = rgb(28, 32, 40);
        const ImVec4 raised     = rgb(36, 41, 52);
        const ImVec4 border     = rgb(46, 52, 64);

        colors[ImGuiCol_Text]                  = rgb(228, 231, 238);
        colors[ImGuiCol_TextDisabled]          = Muted;
        colors[ImGuiCol_WindowBg]              = background;
        colors[ImGuiCol_ChildBg]               = withAlpha(surface, 0.0f);
        colors[ImGuiCol_PopupBg]               = surface;
        colors[ImGuiCol_Border]                = border;
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_FrameBg]               = raised;
        colors[ImGuiCol_FrameBgHovered]        = rgb(46, 53, 68);
        colors[ImGuiCol_FrameBgActive]         = rgb(54, 62, 80);
        colors[ImGuiCol_TitleBg]               = surface;
        colors[ImGuiCol_TitleBgActive]         = raised;
        colors[ImGuiCol_TitleBgCollapsed]      = surface;
        colors[ImGuiCol_MenuBarBg]             = rgb(17, 19, 24);
        colors[ImGuiCol_ScrollbarBg]           = withAlpha(background, 0.0f);
        colors[ImGuiCol_ScrollbarGrab]         = rgb(54, 61, 76);
        colors[ImGuiCol_ScrollbarGrabHovered]  = rgb(68, 77, 96);
        colors[ImGuiCol_ScrollbarGrabActive]   = rgb(82, 93, 116);
        colors[ImGuiCol_CheckMark]             = Accent;
        colors[ImGuiCol_SliderGrab]            = withAlpha(Accent, 0.8f);
        colors[ImGuiCol_SliderGrabActive]      = Accent;
        colors[ImGuiCol_Button]                = raised;
        colors[ImGuiCol_ButtonHovered]         = rgb(50, 58, 74);
        colors[ImGuiCol_ButtonActive]          = rgb(60, 70, 90);
        colors[ImGuiCol_Header]                = withAlpha(Accent, 0.22f);
        colors[ImGuiCol_HeaderHovered]         = withAlpha(Accent, 0.30f);
        colors[ImGuiCol_HeaderActive]          = withAlpha(Accent, 0.40f);
        colors[ImGuiCol_Separator]             = border;
        colors[ImGuiCol_SeparatorHovered]      = withAlpha(Accent, 0.6f);
        colors[ImGuiCol_SeparatorActive]       = Accent;
        colors[ImGuiCol_ResizeGrip]            = withAlpha(Accent, 0.0f);
        colors[ImGuiCol_ResizeGripHovered]     = withAlpha(Accent, 0.5f);
        colors[ImGuiCol_ResizeGripActive]      = Accent;
        colors[ImGuiCol_Tab]                   = surface;
        colors[ImGuiCol_TabHovered]            = withAlpha(Accent, 0.30f);
        colors[ImGuiCol_TabSelected]           = raised;
        colors[ImGuiCol_TabSelectedOverline]   = Accent;
        colors[ImGuiCol_TabDimmed]             = surface;
        colors[ImGuiCol_TabDimmedSelected]     = raised;
        colors[ImGuiCol_TableHeaderBg]         = rgb(30, 35, 45);
        colors[ImGuiCol_TableBorderStrong]     = border;
        colors[ImGuiCol_TableBorderLight]      = rgb(36, 41, 51);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(1.0f, 1.0f, 1.0f, 0.025f);
        colors[ImGuiCol_TextSelectedBg]        = withAlpha(Accent, 0.35f);
        colors[ImGuiCol_TextLink]              = Accent;
        colors[ImGuiCol_NavCursor]             = Accent;
        colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.55f);

        ImPlotStyle& plotStyle = ImPlot::GetStyle();
        plotStyle.UseLocalTime = false; // Plot times are UTC, see YearMonth::toUnixTimestamp()
        plotStyle.UseISO8601 = false;
        plotStyle.Use24HourClock = true;
        plotStyle.Colors[ImPlotCol_FrameBg]   = withAlpha(background, 0.0f);
        plotStyle.Colors[ImPlotCol_PlotBg]    = withAlpha(surface, 0.6f);
        plotStyle.Colors[ImPlotCol_PlotBorder] = withAlpha(border, 0.0f);
        plotStyle.Colors[ImPlotCol_LegendBg]  = withAlpha(surface, 0.85f);
        plotStyle.Colors[ImPlotCol_LegendBorder] = border;
        plotStyle.Colors[ImPlotCol_AxisGrid]  = withAlpha(rgb(120, 130, 150), 0.18f);
    }

}
