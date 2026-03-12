#include "MainMenuState.hpp"
#include "NoteListState.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "db/NoteRepository.hpp"
#include "ui/MainMenu.hpp"
#include <memory>
#include <ncurses.h>

namespace QuickNotes::App::State {

// --- Public Methods ---

MainMenuState::MainMenuState(
    WINDOW *window,
    std::shared_ptr<Config::Config> config,
    IAppController &controller,
    DB::NotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository), m_menu(window) {}

void MainMenuState::render() { m_menu.draw(); }

void MainMenuState::onEnter() { wclear(m_window); }

void MainMenuState::onExit() { wclear(m_window); }

std::unique_ptr<AbstractState> MainMenuState::handleInput(int key) {
  using Action = Config::Action;

  const auto &binds = m_config->keyBinds.bindings;
  auto toAction = [&]() -> MenuAction {
    if (key == binds.at(Action::MOVE_UP)) return MenuAction::MOVE_UP;
    if (key == binds.at(Action::MOVE_DOWN)) return MenuAction::MOVE_DOWN;
    if (key == binds.at(Action::MAIN_MENU_NOTES))
      return MenuAction::NOTES_SELECTED;
    if (key == binds.at(Action::MAIN_MENU_SETTINGS))
      return MenuAction::SETTINGS_SELECTED;
    if (key == binds.at(Action::SELECT)) return MenuAction::SELECT;
    if (key == binds.at(Action::QUIT)) return MenuAction::QUIT_SELECTED;
    return MenuAction::NONE;
  };

  switch (toAction()) {
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
