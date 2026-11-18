#include "FinanceManagerLayer.hpp"

#include "FinanceLayout.hpp"
#include "Theme.hpp"
#include "Ui.hpp"
#include "Popups/ConfirmPopup.hpp"
#include "Popups/CreatePopups.hpp"
#include "Popups/EntryPopups.hpp"
#include "Popups/SettingsPopup.hpp"

#include <VelyraAppFramework/AppData.hpp>
#include <VelyraCore/Window/Window.hpp>

namespace FinMgr {

    FinanceManagerLayer::FinanceManagerLayer(App::AppData& appData):
    Layer(appData),
    m_Persistence(appData.getSettings(), m_Store),
    m_Layout(createSP<FinanceLayout>(m_Store, m_Persistence)) {

    }

    FinanceManagerLayer::~FinanceManagerLayer() = default;

    void FinanceManagerLayer::onAttach() {
        Theme::Dusk::apply(m_AppData.getContext());
        m_Persistence.load();
        attachPopups();
        registerCommands();

        m_AppData.registerLayout(m_Layout);
        m_AppData.setActiveLayout(m_Layout->getID());
    }

    void FinanceManagerLayer::onDetach() {
        // The settings are saved (and synced) by the framework when the application closes
        m_Persistence.commit();
        m_AppData.unregisterLayout(m_Layout->getID());
    }

    void FinanceManagerLayer::onUpdate(const Duration deltaTime) {
        m_Persistence.onUpdate(deltaTime);
    }

    void FinanceManagerLayer::mainMenuBar() {
        m_Commands.drawMainMenuBar([this] { drawStatus(); });
    }

    void FinanceManagerLayer::attachPopups() {
        Popups& popups = m_Store.popups;
        popups.addPaycheck = createSP<AddPaycheckPopup>(m_AppData, m_Store);
        popups.addSavingsEntry = createSP<AddSavingsEntryPopup>(m_AppData, m_Store);
        popups.addExpenseEntry = createSP<AddExpenseEntryPopup>(m_AppData, m_Store);
        popups.createIncome = createSP<CreateIncomePopup>(m_AppData, m_Store);
        popups.createSavingsRecord = createSP<CreateSavingsRecordPopup>(m_AppData, m_Store);
        popups.createMonthlyExpense = createSP<CreateMonthlyExpensePopup>(m_AppData, m_Store);
        popups.confirm = createSP<ConfirmPopup>(m_AppData);
        popups.settings = createSP<SettingsPopup>(m_AppData, m_Store, m_Persistence);

        m_AppData.addPopup(popups.addPaycheck);
        m_AppData.addPopup(popups.addSavingsEntry);
        m_AppData.addPopup(popups.addExpenseEntry);
        m_AppData.addPopup(popups.createIncome);
        m_AppData.addPopup(popups.createSavingsRecord);
        m_AppData.addPopup(popups.createMonthlyExpense);
        m_AppData.addPopup(popups.confirm);
        m_AppData.addPopup(popups.settings);
    }

    void FinanceManagerLayer::registerCommands() {
        // File
        m_Commands.add({
            .id = "file.save",
            .menuPath = "File/Save",
            .run = [this] { m_Persistence.save(); },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_S,
            .enabled = [this] { return !m_Persistence.isReadOnly(); },
        });
        m_Commands.add({
            .id = "file.sync",
            .menuPath = "File/Sync with git",
            .run = [this] { m_Persistence.syncNow(); },
            .shortcut = ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S,
            .enabled = [this] { return m_Persistence.isGitSyncActive() && !m_Persistence.isReadOnly(); },
        });
        m_Commands.addSeparator("File");
        m_Commands.add({
            .id = "file.settings",
            .menuPath = "File/Settings...",
            .run = [this] { m_Store.popups.settings->open(); },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_Comma,
        });
        m_Commands.addSeparator("File");
        m_Commands.add({
            .id = "file.quit",
            .menuPath = "File/Quit",
            .run = [this] { m_AppData.getWindow().close(); },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_Q,
        });

        // Add
        const auto hasWages = [this] { return !m_Store.data.wageManager.getWages().empty(); };
        const auto hasSavings = [this] { return !m_Store.data.savingsManager.getSavingsRecords().empty(); };
        const auto hasExpenses = [this] { return !m_Store.data.monthlyExpensesManager.getMonthlyExpenses().empty(); };
        m_Commands.add({
            .id = "add.paycheck",
            .menuPath = "Add/Paycheck...",
            .run = [this] { m_Store.popups.addPaycheck->open(m_Store.ui.selectedWage); },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_P,
            .enabled = hasWages,
        });
        m_Commands.add({
            .id = "add.savingsEntry",
            .menuPath = "Add/Savings entry...",
            .run = [this] { m_Store.popups.addSavingsEntry->open(m_Store.ui.selectedSavings); },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_E,
            .enabled = hasSavings,
        });
        m_Commands.add({
            .id = "add.expenseEntry",
            .menuPath = "Add/Monthly expense entry...",
            .run = [this] { m_Store.popups.addExpenseEntry->open(""); },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_M,
            .enabled = hasExpenses,
        });
        m_Commands.addSeparator("Add");
        m_Commands.add({
            .id = "add.income",
            .menuPath = "Add/New income source...",
            .run = [this] { m_Store.popups.createIncome->open(); },
        });
        m_Commands.add({
            .id = "add.savings",
            .menuPath = "Add/New savings account...",
            .run = [this] { m_Store.popups.createSavingsRecord->open(); },
        });
        m_Commands.add({
            .id = "add.expense",
            .menuPath = "Add/New monthly expense...",
            .run = [this] { m_Store.popups.createMonthlyExpense->open(); },
        });

        // View
        constexpr ImGuiKey pageKeys[] = {ImGuiKey_1, ImGuiKey_2, ImGuiKey_3, ImGuiKey_4, ImGuiKey_5};
        for (int i = 0; i < static_cast<int>(Page::Count); ++i) {
            const auto page = static_cast<Page>(i);
            m_Commands.add({
                .id = "view.page." + std::to_string(i),
                .menuPath = std::string("View/") + getPageName(page),
                .run = [this, page] { m_Store.ui.setPage(page); },
                .shortcut = ImGuiMod_Ctrl | pageKeys[i],
                .checked = [this, page] { return m_Store.ui.getPage() == page; },
            });
        }
        m_Commands.addSeparator("View");
        m_Commands.add({
            .id = "view.sidebar",
            .menuPath = "View/Sidebar",
            .run = [this] { m_Store.ui.showSidebar = !m_Store.ui.showSidebar; },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_B,
            .checked = [this] { return m_Store.ui.showSidebar; },
        });
        m_Commands.add({
            .id = "view.charts",
            .menuPath = "View/Charts",
            .run = [this] { m_Store.ui.showCharts = !m_Store.ui.showCharts; },
            .shortcut = ImGuiMod_Ctrl | ImGuiKey_G,
            .checked = [this] { return m_Store.ui.showCharts; },
        });
        m_Commands.add({
            .id = "view.resetLayout",
            .menuPath = "View/Reset panel sizes",
            .run = [this] { m_Layout->resetResizes(); },
        });
    }

    void FinanceManagerLayer::drawStatus() const {
        std::string text;
        ImVec4 color = Theme::Muted;
        if (m_Persistence.isReadOnly()) {
            text = "Read-only, data could not be loaded";
            color = Theme::Negative;
        }
        else if (m_Persistence.hasUnsavedChanges()) {
            text = m_Persistence.isGitSyncActive() ? "Unsaved changes (Ctrl+S to sync)" : "Unsaved changes";
            color = Theme::Warning;
        }
        else {
            text = m_Persistence.getStatusMessage().empty() ? "All changes saved" : m_Persistence.getStatusMessage();
        }
        if (const GitSyncDesc gitSync = m_Persistence.getGitSync(); isGitSyncActive(gitSync)) {
            text += "  |  git: " + gitSync.branch;
        }
        const float width = ImGui::CalcTextSize(text.c_str()).x + ImGui::GetStyle().ItemSpacing.x;
        const float available = ImGui::GetContentRegionAvail().x;
        if (available > width) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available - width);
        }
        ImGui::TextColored(color, "%s", text.c_str());
    }

}
