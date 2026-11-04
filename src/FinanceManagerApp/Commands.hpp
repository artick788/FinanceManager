#pragma once

#include <FinanceManager/FinanceManagerInclude.hpp>

#include <functional>
#include <string>
#include <vector>

namespace FinMgr {

    using KeyChord = int; // ImGuiKeyChord, e.g. ImGuiMod_Ctrl | ImGuiKey_S

    struct Command {
        std::string id;        // Unique, e.g. "file.save"
        std::string menuPath;  // Menu and label separated by '/', e.g. "File/Save". Empty to only use the shortcut
        std::function<void()> run;
        KeyChord shortcut = 0;
        std::function<bool()> enabled; // Optional, enabled when empty
        std::function<bool()> checked; // Optional, draws a check mark when it returns true
    };

    /**
     * @brief Every action that shows up in the main menu or has a shortcut. Menus are generated from the menu paths,
     *        in the order the commands were added.
     */
    class CommandRegistry {
    public:
        void add(Command command);

        void addSeparator(const std::string& menu);

        void execute(const std::string& id) const;

        /**
         * @brief Draws the main menu bar and handles shortcuts, call once per frame
         * @param drawRight Drawn right aligned in the menu bar, e.g. a save indicator. Gets the available width.
         */
        void drawMainMenuBar(const std::function<void()>& drawRight) const;

    private:
        void drawMenu(const std::string& prefix, std::vector<std::function<void()>>& triggered) const;

        void processShortcuts(std::vector<std::function<void()>>& triggered) const;

        static bool isEnabled(const Command& command);

    private:
        std::vector<Command> m_Commands;
    };

}
