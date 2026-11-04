#pragma once

#include <FinanceManager/Date.hpp>
#include <VelyraCore/ImGui/ImGui.hpp>

#include <string>
#include <vector>

/*
 * Small widgets shared by all pages, so every page looks and behaves the same.
 */

namespace FinMgr::Ui {

    /**
     * @brief The currency appended to every formatted amount, set once per frame from the UI state
     */
    void setCurrency(const std::string& currency);

    /**
     * @return The amount with thousands separators and 2 decimals, e.g. "-1,234.50 €"
     */
    std::string money(double amount);

    /**
     * @return The amount as a signed change, e.g. "+120.00 €"
     */
    std::string signedMoney(double amount);

    std::string percentage(double fraction);

    const char* monthName(int month);

    const char* monthShortName(int month);

    /**
     * @return e.g. "March 2026"
     */
    std::string monthYear(int month, int year);

    /**
     * @return e.g. "25 Mar"
     */
    std::string dayMonth(const Date& date);

    // ----------------------------------------------------------------------------------------------------------------
    // Text

    /**
     * @brief Large title of a panel with an optional muted subtitle next to it
     */
    void panelTitle(const char* title, const char* subtitle = nullptr);

    /**
     * @brief Moves the cursor so the following items, with the given total width, end at the right edge. Call after
     *        the item they should share the line with.
     */
    void sameLineRight(float itemsWidth);

    float buttonWidth(const char* label);

    /**
     * @param inset Space kept free at the right edge, use the frame padding to line up with amountInput()
     */
    void textRightAligned(const char* text, const ImVec4* color = nullptr, float inset = 0.0f);

    /**
     * @brief Right aligned amount, colored green/red by sign if colorBySign is true
     */
    void amount(double value, bool colorBySign = false);

    void helpMarker(const char* text);

    /**
     * @brief Centered message for panels without data, with an optional call to action
     * @return true if the button was clicked
     */
    bool emptyState(const char* title, const char* message, const char* buttonLabel = nullptr);

    /**
     * @brief Colored banner across the panel width, e.g. for errors
     */
    void banner(const ImVec4& color, const char* text);

    // ----------------------------------------------------------------------------------------------------------------
    // Cards

    struct Kpi {
        const char* label = "";
        std::string value;
        std::string detail;  // Small text below the value, optional
        ImVec4 valueColor = ImVec4(0, 0, 0, 0); // Transparent uses the text color
        ImVec4 detailColor = ImVec4(0, 0, 0, 0);
    };

    /**
     * @brief Draws cards next to each other, wrapping to the next line when the panel is too narrow
     */
    void kpiRow(const char* id, const Kpi* kpis, int count, float minCardWidth = 180.0f);

    // ----------------------------------------------------------------------------------------------------------------
    // Tables

    /**
     * @brief Highlights the current table row
     */
    void rowBackground(const ImVec4& color);

    /**
     * @brief Editable amount that looks like plain text until hovered, so tables are not a wall of boxes
     * @return true if the value was changed
     */
    bool amountInput(const char* id, double* value);

    // ----------------------------------------------------------------------------------------------------------------
    // Inputs

    /**
     * @brief Day, month and year input that always produces a valid date
     * @return true if the date was changed
     */
    bool dateInput(const char* id, Date& date, bool withDay = true);

    /**
     * @brief Label on the left, input on the right, for tidy forms
     */
    void formLabel(const char* label, float labelWidth = 170.0f);

    // ----------------------------------------------------------------------------------------------------------------
    // Plots

    /**
     * @brief Begins a plot with months on the x axis and amounts on the y axis
     */
    bool beginTimePlot(const char* id, const ImVec2& size = ImVec2(-1.0f, -1.0f), bool showLegend = true);

    /**
     * @return The index of the x value closest to the mouse when the current plot is hovered, -1 otherwise
     */
    int hoveredIndex(const std::vector<double>& xs);

    /**
     * @brief Vertical line at the hovered x value, call before EndPlot
     */
    void hoverLine(double x);

}
