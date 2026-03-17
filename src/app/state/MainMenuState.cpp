#include "MainMenuState.hpp"
#include "NoteListState.hpp"
#include "StateUtils.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "ui/MainMenu.hpp"
#include "utils/Log.hpp"
#include <algorithm>
#include <memory>
#include <ncurses.h>
#include <stdexcept>

namespace QuickNotes::App::State {

// --- Public Methods ---

MainMenuState::MainMenuState(
    WINDOW *window,
    std::shared_ptr<const Config::Config> config,
    IAppController &controller,
    DB::INotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository),
      m_menu(window) {}

void MainMenuState::onEnter() {
  wclear(m_window);
  try {
    m_recentNotes = m_repository.findMostRecent(5);
  } catch (const std::runtime_error &e) {
    QN_LOG_ERROR("MainMenuState: failed to load recent notes: {}", e.what());
    m_recentNotes = {};
  }
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
  auto action =
      dispatchKey(key, m_config->keyBinds, m_keyMap, MenuAction::NONE);
  switch (action) {
    case MenuAction::MOVE_UP: moveUp(); return nullptr;
    case MenuAction::MOVE_DOWN: moveDown(); return nullptr;
    case MenuAction::SELECT:
      {
        if (m_recentNotes.empty()) {
          return nullptr;
        }
        return makeState<NoteListState>(m_repository);
      }
    case MenuAction::NOTES_SELECTED:
      return makeState<NoteListState>(m_repository);
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
