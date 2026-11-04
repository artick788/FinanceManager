#include "Commands.hpp"

#include <VelyraCore/ImGui/ImGui.hpp>
#include <VelyraCore/ImGui/imgui_internal.h> // GetKeyChordName

#include <stdexcept>

namespace FinMgr {

    namespace {

        /**
         * @return The part of the path after prefix up to the next '/', and whether that part is the leaf (a label)
         */
        std::pair<std::string, bool> nextSegment(const std::string& path, const std::string& prefix) {
            const std::string rest = path.substr(prefix.size());
            const std::size_t slash = rest.find('/');
            if (slash == std::string::npos) {
                return {rest, true};
            }
            return {rest.substr(0, slash), false};
        }

    }

    void CommandRegistry::add(Command command) {
        if (std::ranges::any_of(m_Commands, [&](const Command& c) { return c.id == command.id; })) {
            throw std::logic_error("Command registered twice: " + command.id);
        }
        m_Commands.push_back(std::move(command));
    }

    void CommandRegistry::addSeparator(const std::string& menu) {
        m_Commands.push_back({.id = "separator." + std::to_string(m_Commands.size()), .menuPath = menu + "/-", .run = [] {}});
    }

    void CommandRegistry::execute(const std::string& id) const {
        const auto it = std::ranges::find_if(m_Commands, [&](const Command& c) { return c.id == id; });
        if (it != m_Commands.end() && isEnabled(*it)) {
            it->run();
        }
    }

    void CommandRegistry::drawMainMenuBar(const std::function<void()>& drawRight) const {
        // Commands run after drawing, so a command may change anything without breaking the menu
        std::vector<std::function<void()>> triggered;
        processShortcuts(triggered);
        if (ImGui::BeginMainMenuBar()) {
            std::vector<std::string> menus;
            for (const auto& command: m_Commands) {
                if (const auto [segment, leaf] = nextSegment(command.menuPath, ""); !leaf && std::ranges::find(menus, segment) == menus.end()) {
                    menus.push_back(segment);
                }
            }
            for (const auto& menu: menus) {
                if (ImGui::BeginMenu(menu.c_str())) {
                    drawMenu(menu + "/", triggered);
                    ImGui::EndMenu();
                }
            }
            if (drawRight) {
                drawRight();
            }
            ImGui::EndMainMenuBar();
        }
        for (const auto& run: triggered) {
            run();
        }
    }

    void CommandRegistry::drawMenu(const std::string& prefix, std::vector<std::function<void()>>& triggered) const {
        std::vector<std::string> drawnSubMenus;
        for (const auto& command: m_Commands) {
            if (!command.menuPath.starts_with(prefix)) {
                continue;
            }
            const auto [segment, leaf] = nextSegment(command.menuPath, prefix);
            if (leaf) {
                if (segment == "-") {
                    ImGui::Separator();
                    continue;
                }
                const std::string shortcut = command.shortcut ? ImGui::GetKeyChordName(command.shortcut) : "";
                const bool checked = command.checked && command.checked();
                if (ImGui::MenuItem(segment.c_str(), shortcut.empty() ? nullptr : shortcut.c_str(), checked, isEnabled(command))) {
                    triggered.push_back(command.run);
                }
            }
            else if (std::ranges::find(drawnSubMenus, segment) == drawnSubMenus.end()) {
                drawnSubMenus.push_back(segment);
                if (ImGui::BeginMenu(segment.c_str())) {
                    drawMenu(prefix + segment + "/", triggered);
                    ImGui::EndMenu();
                }
            }
        }
    }

    void CommandRegistry::processShortcuts(std::vector<std::function<void()>>& triggered) const {
        // Shortcuts would fire while typing in a modal, only handle them when no popup is open
        if (ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopupId | ImGuiPopupFlags_AnyPopupLevel)) {
            return;
        }
        for (const auto& command: m_Commands) {
            if (command.shortcut && ImGui::Shortcut(command.shortcut, ImGuiInputFlags_RouteGlobal) && isEnabled(command)) {
                triggered.push_back(command.run);
            }
        }
    }

    bool CommandRegistry::isEnabled(const Command& command) {
        return !command.enabled || command.enabled();
    }

}
