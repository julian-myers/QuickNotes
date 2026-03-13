#include "MainMenuState.hpp"
#include "NoteListState.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "db/NoteRepository.hpp"
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
    DB::NotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository), m_menu(window) {}

void MainMenuState::render() { m_menu.draw(); }

void MainMenuState::onEnter() { wclear(m_window); }

void MainMenuState::onExit() { wclear(m_window); }

using Action = Config::Action;
const std::vector<std::pair<Action, MainMenuState::MenuAction>>
    MainMenuState::m_keyMap{
        {Action::MOVE_UP, MenuAction::MOVE_UP},
        {Action::MOVE_DOWN, MenuAction::MOVE_DOWN},
        {Action::MAIN_MENU_NOTES, MenuAction::NOTES_SELECTED},
        {Action::MAIN_MENU_SETTINGS, MenuAction::SETTINGS_SELECTED},
        {Action::SELECT, MenuAction::SELECT}
    };

std::unique_ptr<AbstractState> MainMenuState::handleInput(int key) {
  const auto &binds = m_config->keyBinds.bindings;
  auto it = std::ranges::find_if(m_keyMap, [&](const auto &pair) {
    return key == binds.at(pair.first);
  });
  auto action = (it != m_keyMap.end()) ? it->second : MenuAction::NONE;
  switch (action) {
    case MenuAction::MOVE_UP:
      moveUp();
      return nullptr;
    case MenuAction::MOVE_DOWN:
      moveDown();
      return nullptr;
    case MenuAction::SELECT:
      return nullptr;
    case MenuAction::NOTES_SELECTED:
      return makeNoteAwareState<NoteListState>();
    case MenuAction::SETTINGS_SELECTED:
      return nullptr;
    case MenuAction::QUIT_SELECTED:
      m_controller.quit();
      return nullptr;
    case MenuAction::NONE:
      return nullptr;
    default:
      return nullptr;
  }
}

// ---- Private Methods -----

void MainMenuState::moveDown() {
  m_selectedIndex = std::min(NUM_OPTIONS - 1, m_selectedIndex + 1);
}

} // namespace QuickNotes::App::State
