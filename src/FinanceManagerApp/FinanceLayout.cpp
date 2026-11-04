#include "FinanceLayout.hpp"

#include "Popups/EntryPopups.hpp"
#include "Theme.hpp"
#include "Ui.hpp"

#include <ranges>

namespace FinMgr {

    const char* getPageName(const Page page) {
        switch (page) {
            case Page::Dashboard: return "Dashboard";
            case Page::Income: return "Income";
            case Page::Savings: return "Savings";
            case Page::Expenses: return "Monthly expenses";
            case Page::Manage: return "Manage";
            default: return "?";
        }
    }

    namespace {

        template<typename MAP>
        std::vector<std::string> sortedNames(const MAP& map) {
            std::vector<std::string> names;
            for (const auto& name: map | std::views::keys) {
                names.push_back(name);
            }
            std::ranges::sort(names);
            return names;
        }

        bool subItem(const std::string& label, const bool selected) {
            ImGui::Indent();
            ImGui::PushStyleColor(ImGuiCol_Text, selected ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : Theme::Muted);
            const bool clicked = ImGui::Selectable(label.c_str(), selected);
            ImGui::PopStyleColor();
            ImGui::Unindent();
            return clicked;
        }

    }

    FinanceLayout::FinanceLayout(Store& store, Persistence& persistence):
    m_Store(store),
    m_Persistence(persistence),
    m_Dashboard(store),
    m_Income(store),
    m_Savings(store),
    m_Expenses(store),
    m_Manage(store) {

    }

    SP<App::Node> FinanceLayout::getLayout() {
        using namespace App;
        Ui::setCurrency(m_Store.ui.currency);

        SP<Node> content = buildPage();
        if (hasNotice()) {
            content = verticalSplit({.name = "Content"},
                createPanel({.name = "Notice", .drawFunction = bindDraw(this, &FinanceLayout::drawNotice), .resizable = false, .sizeRatio = 0.075f}),
                content);
        }
        if (!m_Store.ui.showSidebar) {
            return content;
        }
        return horizontalSplit({.name = "Root"},
            createPanel({.name = "Navigator", .drawFunction = bindDraw(this, &FinanceLayout::drawNavigator), .sizeRatio = 0.16f}),
            content);
    }

    SP<App::Node> FinanceLayout::buildPage() {
        switch (m_Store.ui.getPage()) {
            case Page::Income: return m_Income.buildLayout();
            case Page::Savings: return m_Savings.buildLayout();
            case Page::Expenses: return m_Expenses.buildLayout();
            case Page::Manage: return m_Manage.buildLayout();
            case Page::Dashboard:
            default: return m_Dashboard.buildLayout();
        }
    }

    bool FinanceLayout::hasNotice() const {
        return m_Persistence.isReadOnly() || (m_Persistence.wasMigrated() && !m_MigrationNoticeDismissed);
    }

    void FinanceLayout::drawNotice() {
        if (m_Persistence.isReadOnly()) {
            const std::string text = "Your data could not be read, so saving is disabled to keep the file intact. "
                "Fix or restore " + getSettingsFilePath().string() + " and restart the app. Error: " + m_Persistence.getLoadError();
            Ui::banner(Theme::Negative, text.c_str());
            return;
        }
        Ui::banner(Theme::Positive, "Your data was imported from the previous version of FinanceManager. "
                                    "Nothing changed in how it is stored, a backup of the original file is kept next to it.");
        ImGui::SameLine();
        if (ImGui::SmallButton("Dismiss")) {
            m_MigrationNoticeDismissed = true;
        }
    }

    // -----------------------------------------------------------------------------------------------------------------

    void FinanceLayout::drawNavigator() {
        ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * 1.3f);
        ImGui::TextUnformatted("FinanceManager");
        ImGui::PopFont();
        const Date today = getCurrentDate();
        ImGui::TextDisabled("%d %s %d", today.day, Ui::monthName(today.month), today.year);
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        drawNavigatorItem(Page::Dashboard, "Ctrl+1");

        drawNavigatorItem(Page::Income, "Ctrl+2");
        const std::string* selectedWage = m_Income.getSelectedWage();
        for (const auto& name: sortedNames(m_Store.data.wageManager.getWages())) {
            const bool selected = m_Store.ui.getPage() == Page::Income && selectedWage != nullptr && *selectedWage == name;
            if (subItem(name + "##Wage", selected)) {
                m_Store.ui.selectedWage = name;
                m_Store.ui.setPage(Page::Income);
            }
        }

        drawNavigatorItem(Page::Savings, "Ctrl+3");
        if (!m_Store.data.savingsManager.getSavingsRecords().empty()) {
            const std::string* selectedRecord = m_Savings.getSelectedRecord();
            const bool onSavings = m_Store.ui.getPage() == Page::Savings;
            if (subItem("All accounts##Savings", onSavings && selectedRecord == nullptr)) {
                m_Store.ui.selectedSavings.clear();
                m_Store.ui.setPage(Page::Savings);
            }
            for (const auto& name: sortedNames(m_Store.data.savingsManager.getSavingsRecords())) {
                if (subItem(name + "##Savings", onSavings && selectedRecord != nullptr && *selectedRecord == name)) {
                    m_Store.ui.selectedSavings = name;
                    m_Store.ui.setPage(Page::Savings);
                }
            }
        }

        drawNavigatorItem(Page::Expenses, "Ctrl+4");
        drawNavigatorItem(Page::Manage, "Ctrl+5");

        drawNavigatorFooter();
    }

    void FinanceLayout::drawNavigatorItem(const Page page, const char* shortcut) {
        const bool selected = m_Store.ui.getPage() == page;
        ImGui::PushID(static_cast<int>(page));
        if (ImGui::Selectable(getPageName(page), selected, ImGuiSelectableFlags_AllowOverlap)) {
            m_Store.ui.setPage(page);
        }
        ImGui::SameLine();
        Ui::textRightAligned(shortcut, &Theme::Muted);
        ImGui::PopID();
    }

    void FinanceLayout::drawNavigatorFooter() {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float buttonHeight = ImGui::GetFrameHeight();
        const float footerHeight = ImGui::GetTextLineHeightWithSpacing() * 2.0f + (buttonHeight + style.ItemSpacing.y) * 3.0f + style.ItemSpacing.y;
        const float footerY = ImGui::GetWindowHeight() - style.WindowPadding.y - footerHeight;
        if (footerY > ImGui::GetCursorPosY()) {
            ImGui::SetCursorPosY(footerY);
        }
        else {
            ImGui::Spacing();
        }
        ImGui::SeparatorText("Quick add");

        const bool hasWages = !m_Store.data.wageManager.getWages().empty();
        const bool hasSavings = !m_Store.data.savingsManager.getSavingsRecords().empty();
        const bool hasExpenses = !m_Store.data.monthlyExpensesManager.getMonthlyExpenses().empty();
        const ImVec2 size(-FLT_MIN, 0.0f);

        ImGui::BeginDisabled(!hasWages);
        if (ImGui::Button("Paycheck", size)) {
            const std::string* wage = m_Income.getSelectedWage();
            m_Store.popups.addPaycheck->open(wage != nullptr ? *wage : "");
        }
        ImGui::EndDisabled();
        ImGui::BeginDisabled(!hasSavings);
        if (ImGui::Button("Savings entry", size)) {
            const std::string* record = m_Savings.getSelectedRecord();
            m_Store.popups.addSavingsEntry->open(record != nullptr ? *record : "");
        }
        ImGui::EndDisabled();
        ImGui::BeginDisabled(!hasExpenses);
        if (ImGui::Button("Expense entry", size)) {
            m_Store.popups.addExpenseEntry->open("");
        }
        ImGui::EndDisabled();
    }

}
