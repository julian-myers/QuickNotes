#include "MainMenuState.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "app/state/NoteListState.hpp"
#include "app/state/ViewingState.hpp"
#include "config/Config.hpp"
#include "ui/MainMenu.hpp"
#include <algorithm>
#include <memory>
#include <ncurses.h>

namespace QuickNotes::App::State {

// --- Public Methods ---

MainMenuState::MainMenuState(
    WINDOW *window,
    std::shared_ptr<const Config::Config> config,
    IAppController &controller,
    DB::INotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository), m_menu(window) {}

void MainMenuState::onEnter() {
  wclear(m_window);
  m_recentNotes = m_repository.findMostRecent(5);
  m_menu.setRecentNotes(m_recentNotes);
}

void MainMenuState::render() {
  m_menu.setNotesIndex(m_selectedIndex);
  m_menu.draw();
}

void MainMenuState::onExit() { wclear(m_window); }

using Action = Config::Action;
const std::vector<std::pair<Action, MainMenuState::MenuAction>>
    MainMenuState::m_keyMap{
        {Action::MOVE_UP, MenuAction::MOVE_UP},
        {Action::MOVE_DOWN, MenuAction::MOVE_DOWN},
        {Action::MAIN_MENU_NOTES, MenuAction::NOTES_SELECTED},
        {Action::MAIN_MENU_SETTINGS, MenuAction::SETTINGS_SELECTED},
        {Action::QUIT, MenuAction::QUIT_SELECTED},
        {Action::SELECT, MenuAction::SELECT}
    };

std::unique_ptr<AbstractState> MainMenuState::handleInput(int key) {
  const auto &binds = m_config->keyBinds.bindings;
  auto it = std::ranges::find_if(m_keyMap, [&](const auto &pair) {
    return key == binds.at(pair.first);
  });
  auto action = (it != m_keyMap.end()) ? it->second : MenuAction::NONE;
  switch (action) {
    case MenuAction::MOVE_UP: moveUp(); return nullptr;
    case MenuAction::MOVE_DOWN: moveDown(); return nullptr;
    case MenuAction::SELECT:
      if (m_recentNotes.empty()) return nullptr;
      return std::make_unique<ViewingState>(
          m_window,
          m_config,
          m_controller,
          m_repository,
          m_recentNotes[m_selectedIndex]
      );
    case MenuAction::NOTES_SELECTED: return makeNoteAwareState<NoteListState>();
    case MenuAction::SETTINGS_SELECTED: return nullptr;
    case MenuAction::QUIT_SELECTED: m_controller.quit(); return nullptr;
    case MenuAction::NONE: return nullptr;
    default: return nullptr;
  }
}

// ---- Private Methods -----

void MainMenuState::moveUp() {
  m_selectedIndex = std::max(0, m_selectedIndex - 1);
}
void MainMenuState::moveDown() {
  m_selectedIndex =
      std::min(static_cast<int>(m_recentNotes.size() - 1), m_selectedIndex + 1);
}

} // namespace QuickNotes::App::State
