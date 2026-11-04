#include "Ui.hpp"
#include "Theme.hpp"

#include <cmath>
#include <cstdio>

namespace FinMgr::Ui {

    namespace {

        std::string s_Currency;

        bool isSet(const ImVec4& color) {
            return color.w > 0.0f;
        }

        float scaled(const float value) {
            const float scale = ImGui::GetStyle().FontScaleDpi;
            return value * (scale > 0.0f ? scale : 1.0f);
        }

        void pushFontScale(const float factor) {
            ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * factor);
        }

    }

    void setCurrency(const std::string& currency) {
        s_Currency = currency;
    }

    std::string money(const double amount) {
        // Avoid showing "-0.00"
        const double rounded = std::round(amount * 100.0) / 100.0;
        const double value = rounded == 0.0 ? 0.0 : rounded;

        char digits[64];
        std::snprintf(digits, sizeof(digits), "%.2f", std::fabs(value));
        std::string integer(digits);
        const std::size_t dot = integer.find('.');
        const std::string decimals = integer.substr(dot);
        integer = integer.substr(0, dot);

        std::string grouped;
        for (std::size_t i = 0; i < integer.size(); ++i) {
            if (i > 0 && (integer.size() - i) % 3 == 0) {
                grouped += ',';
            }
            grouped += integer[i];
        }
        std::string result = (value < 0.0 ? "-" : "") + grouped + decimals;
        if (!s_Currency.empty()) {
            result += " " + s_Currency;
        }
        return result;
    }

    std::string signedMoney(const double amount) {
        const std::string text = money(amount);
        return std::round(amount * 100.0) > 0.0 ? "+" + text : text;
    }

    std::string percentage(const double fraction) {
        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "%.1f%%", fraction * 100.0);
        return buffer;
    }

    const char* monthName(const int month) {
        static const char* months[] = {
            "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"
        };
        return month >= 1 && month <= 12 ? months[month - 1] : "?";
    }

    const char* monthShortName(const int month) {
        static const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        return month >= 1 && month <= 12 ? months[month - 1] : "?";
    }

    std::string monthYear(const int month, const int year) {
        return std::string(monthName(month)) + " " + std::to_string(year);
    }

    std::string dayMonth(const Date& date) {
        return std::to_string(date.day) + " " + monthShortName(date.month);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void panelTitle(const char* title, const char* subtitle) {
        pushFontScale(1.35f);
        ImGui::TextUnformatted(title);
        ImGui::PopFont();
        if (subtitle != nullptr && subtitle[0] != '\0') {
            ImGui::SameLine();
            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("%s", subtitle);
        }
    }

    void sameLineRight(const float itemsWidth) {
        ImGui::SameLine();
        const float available = ImGui::GetContentRegionAvail().x;
        if (available > itemsWidth) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available - itemsWidth);
        }
    }

    float buttonWidth(const char* label) {
        return ImGui::CalcTextSize(label, nullptr, true).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    }

    void textRightAligned(const char* text, const ImVec4* color, const float inset) {
        const float width = ImGui::CalcTextSize(text).x + inset;
        const float available = ImGui::GetContentRegionAvail().x;
        if (available > width) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available - width);
        }
        if (color != nullptr) {
            ImGui::TextColored(*color, "%s", text);
        }
        else {
            ImGui::TextUnformatted(text);
        }
    }

    void amount(const double value, const bool colorBySign) {
        const std::string text = colorBySign ? signedMoney(value) : money(value);
        const double cents = std::round(value * 100.0);
        // Inset like the text of amountInput(), so read only and editable amounts line up
        const float inset = ImGui::GetStyle().FramePadding.x;
        if (colorBySign && cents != 0.0) {
            const ImVec4& color = cents > 0.0 ? Theme::Positive : Theme::Negative;
            textRightAligned(text.c_str(), &color, inset);
        }
        else {
            textRightAligned(text.c_str(), nullptr, inset);
        }
    }

    void helpMarker(const char* text) {
        ImGui::TextDisabled("(?)");
        if (ImGui::BeginItemTooltip()) {
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 30.0f);
            ImGui::TextUnformatted(text);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    bool emptyState(const char* title, const char* message, const char* buttonLabel) {
        const ImVec2 available = ImGui::GetContentRegionAvail();
        const float wrapWidth = std::min(available.x * 0.8f, scaled(420.0f));
        const ImVec2 messageSize = ImGui::CalcTextSize(message, nullptr, false, wrapWidth);
        const float height = ImGui::GetFontSize() * 2.5f + messageSize.y + (buttonLabel ? ImGui::GetFrameHeightWithSpacing() * 1.5f : 0.0f);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + std::max(0.0f, (available.y - height) * 0.4f));

        const auto centerX = [&](const float width) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (available.x - width) * 0.5f));
        };
        pushFontScale(1.25f);
        centerX(ImGui::CalcTextSize(title).x);
        ImGui::TextUnformatted(title);
        ImGui::PopFont();

        centerX(messageSize.x);
        const float startX = ImGui::GetCursorPosX();
        ImGui::PushTextWrapPos(startX + wrapWidth);
        ImGui::PushStyleColor(ImGuiCol_Text, Theme::Muted);
        ImGui::TextWrapped("%s", message);
        ImGui::PopStyleColor();
        ImGui::PopTextWrapPos();

        if (buttonLabel == nullptr) {
            return false;
        }
        ImGui::Spacing();
        centerX(buttonWidth(buttonLabel));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(Theme::Accent.x, Theme::Accent.y, Theme::Accent.z, 0.75f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, Theme::Accent);
        const bool clicked = ImGui::Button(buttonLabel);
        ImGui::PopStyleColor(2);
        return clicked;
    }

    void banner(const ImVec4& color, const char* text) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(color.x, color.y, color.z, 0.18f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(color.x, color.y, color.z, 0.6f));
        if (ImGui::BeginChild("##Banner", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)) {
            ImGui::TextWrapped("%s", text);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
    }

    // -----------------------------------------------------------------------------------------------------------------

    void kpiRow(const char* id, const Kpi* kpis, const int count, const float minCardWidth) {
        if (count <= 0) {
            return;
        }
        ImGui::PushID(id);
        const ImGuiStyle& style = ImGui::GetStyle();
        const float available = ImGui::GetContentRegionAvail().x;
        const float minWidth = scaled(minCardWidth);
        const int perRow = std::clamp(static_cast<int>((available + style.ItemSpacing.x) / (minWidth + style.ItemSpacing.x)), 1, count);
        const float cardWidth = (available - style.ItemSpacing.x * static_cast<float>(perRow - 1)) / static_cast<float>(perRow);

        ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::CardBg);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, scaled(8.0f));
        for (int i = 0; i < count; ++i) {
            const Kpi& kpi = kpis[i];
            if (i % perRow != 0) {
                ImGui::SameLine();
            }
            ImGui::PushID(i);
            if (ImGui::BeginChild("##Card", ImVec2(cardWidth, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)) {
                ImGui::TextDisabled("%s", kpi.label);
                pushFontScale(1.6f);
                if (isSet(kpi.valueColor)) {
                    ImGui::TextColored(kpi.valueColor, "%s", kpi.value.c_str());
                }
                else {
                    ImGui::TextUnformatted(kpi.value.c_str());
                }
                ImGui::PopFont();
                if (!kpi.detail.empty()) {
                    ImGui::TextColored(isSet(kpi.detailColor) ? kpi.detailColor : Theme::Muted, "%s", kpi.detail.c_str());
                }
                else {
                    // Keep the cards of one row equally high
                    ImGui::TextUnformatted(" ");
                }
            }
            ImGui::EndChild();
            ImGui::PopID();
        }
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        ImGui::PopID();
    }

    // -----------------------------------------------------------------------------------------------------------------

    void rowBackground(const ImVec4& color) {
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(color));
    }

    bool amountInput(const char* id, double* value) {
        // Shown as a formatted, right aligned amount like every other amount, it turns into an input when clicked
        ImGuiStorage* storage = ImGui::GetStateStorage();
        const ImGuiID editingKey = ImGui::GetID(id);
        ImGui::PushID(id);
        const ImGuiID focusKey = ImGui::GetID("##Focus");
        ImGui::PopID();

        bool changed = false;
        if (storage->GetBool(editingKey)) {
            const bool justStarted = storage->GetBool(focusKey);
            if (justStarted) {
                ImGui::SetKeyboardFocusHere();
                storage->SetBool(focusKey, false);
            }
            ImGui::SetNextItemWidth(-FLT_MIN);
            changed = ImGui::InputDouble(id, value, 0.0, 0.0, "%.2f", ImGuiInputTextFlags_AutoSelectAll);
            if (!justStarted && !ImGui::IsItemActive()) {
                storage->SetBool(editingKey, false);
            }
            return changed;
        }

        const ImVec2 position = ImGui::GetCursorScreenPos();
        const ImVec2 size(std::max(ImGui::GetContentRegionAvail().x, 1.0f), ImGui::GetFrameHeight());
        if (ImGui::InvisibleButton(id, size)) {
            storage->SetBool(editingKey, true);
            storage->SetBool(focusKey, true);
        }
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (ImGui::IsItemHovered()) {
            ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
            drawList->AddRectFilled(position, ImVec2(position.x + size.x, position.y + size.y),
                ImGui::GetColorU32(ImGuiCol_FrameBgHovered), ImGui::GetStyle().FrameRounding);
        }
        const std::string text = money(*value);
        const ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
        const float padding = ImGui::GetStyle().FramePadding.x;
        const ImVec2 textPosition(std::max(position.x + padding, position.x + size.x - textSize.x - padding),
                                  position.y + (size.y - textSize.y) * 0.5f);
        drawList->AddText(textPosition, ImGui::GetColorU32(ImGuiCol_Text), text.c_str());
        return false;
    }

    // -----------------------------------------------------------------------------------------------------------------

    bool dateInput(const char* id, Date& date, const bool withDay) {
        ImGui::PushID(id);
        bool changed = false;
        const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        const float available = ImGui::GetContentRegionAvail().x;
        const float dayWidth = withDay ? scaled(90.0f) : 0.0f;
        const float yearWidth = scaled(110.0f);
        const float monthWidth = std::max(scaled(100.0f), available - dayWidth - yearWidth - spacing * (withDay ? 2.0f : 1.0f));

        bool dayChanged = false;
        if (withDay) {
            ImGui::SetNextItemWidth(dayWidth);
            dayChanged = ImGui::InputInt("##Day", &date.day);
            changed |= dayChanged;
            ImGui::SameLine(0.0f, spacing);
        }
        ImGui::SetNextItemWidth(monthWidth);
        if (ImGui::BeginCombo("##Month", monthName(date.month))) {
            for (int month = 1; month <= 12; ++month) {
                if (ImGui::Selectable(monthName(month), month == date.month)) {
                    date.month = month;
                    changed = true;
                }
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine(0.0f, spacing);
        ImGui::SetNextItemWidth(yearWidth);
        changed |= ImGui::InputInt("##Year", &date.year);

        date.year = std::clamp(date.year, 1970, 9999);
        date.month = std::clamp(date.month, 1, 12);
        if (withDay) {
            const int days = daysInMonth(date.month, date.year);
            // Stepping the day past the end of the month rolls over into the next/previous month, like the old date
            // input did. Typed values and month changes keep the day within the month.
            if (dayChanged && date.day == 0) {
                date.month--;
                if (date.month < 1) {
                    date.month = 12;
                    date.year--;
                }
                date.day = daysInMonth(date.month, date.year);
            }
            else if (dayChanged && date.day == days + 1) {
                date.day = 1;
                date.monthIncrement();
            }
            date.day = std::clamp(date.day, 1, daysInMonth(date.month, date.year));
        }
        ImGui::PopID();
        return changed;
    }

    void formLabel(const char* label, const float labelWidth) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine(scaled(labelWidth));
        ImGui::SetNextItemWidth(-FLT_MIN);
    }

    // -----------------------------------------------------------------------------------------------------------------

    namespace {

        int formatAxisAmount(const double value, char* buffer, const int size, void*) {
            const double magnitude = std::fabs(value);
            if (magnitude >= 1e6) {
                return std::snprintf(buffer, size, "%.1fM", value / 1e6);
            }
            if (magnitude >= 1e3) {
                return std::snprintf(buffer, size, "%.1fk", value / 1e3);
            }
            return std::snprintf(buffer, size, "%.0f", value);
        }

    }

    bool beginTimePlot(const char* id, const ImVec2& size, const bool showLegend) {
        ImPlotFlags flags = ImPlotFlags_NoMouseText | ImPlotFlags_NoBoxSelect;
        if (!showLegend) {
            flags |= ImPlotFlags_NoLegend;
        }
        if (!ImPlot::BeginPlot(id, size, flags)) {
            return false;
        }
        ImPlot::SetupAxes(nullptr, nullptr, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoGridLines, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisFormat(ImAxis_Y1, formatAxisAmount);
        if (showLegend) {
            ImPlot::SetupLegend(ImPlotLocation_NorthWest);
        }
        return true;
    }

    int hoveredIndex(const std::vector<double>& xs) {
        if (xs.empty() || !ImPlot::IsPlotHovered()) {
            return -1;
        }
        const double mouseX = ImPlot::GetPlotMousePos().x;
        int best = 0;
        for (int i = 1; i < static_cast<int>(xs.size()); ++i) {
            if (std::fabs(xs[i] - mouseX) < std::fabs(xs[best] - mouseX)) {
                best = i;
            }
        }
        return best;
    }

    void hoverLine(const double x) {
        ImPlotSpec spec;
        spec.LineColor = ImVec4(1.0f, 1.0f, 1.0f, 0.25f);
        spec.Flags = ImPlotItemFlags_NoLegend | ImPlotItemFlags_NoFit;
        ImPlot::PlotInfLines("##Hover", &x, 1, spec);
    }

}
