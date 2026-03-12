#include "ViewingState.hpp"
#include "app/Controller.hpp"
#include "config/Editor.hpp"
#include "db/NoteRepository.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/NoteViewWidget.hpp"
#include <memory>

namespace QuickNotes::App::State {

using ConfigPtr = std::shared_ptr<Config::Config>;

ViewingState::ViewingState(WINDOW *window, ConfigPtr config,
                           IAppController &controller,
                           DB::NotesRepository &repository,
                           Model::Note note) noexcept
    : NoteAwareState(window, config, controller, repository), m_note(note) {}

void ViewingState::onEnter() {
  wclear(m_window);
  m_scrollOffset = 0;
  int width = getmaxx(m_window);
  m_pad = newpad(PAD_HEIGHT, width);
  m_widget = std::make_unique<UI::NoteViewWidget>(m_pad, m_config, m_note);
  m_widget->draw();
}

void ViewingState::render() {
  int height = getmaxy(m_window);
  int width = getmaxx(m_window);
  prefresh(m_pad, m_scrollOffset, 0, 0, 0, height - 1, width - 1);
}

void ViewingState::onExit() {
  if (m_pad) {
    delwin(m_pad);
    m_pad = nullptr;
  }
  wclear(m_window);
}

void ViewingState::moveUp() {
  m_scrollOffset = std::max(0, m_scrollOffset - 1);
}

void ViewingState::moveDown() {
  m_scrollOffset = std::min(PAD_HEIGHT - getmaxy(m_window), m_scrollOffset + 1);
}

std::unique_ptr<AbstractState> ViewingState::handleInput(int key) {
  using Action = Config::Action;
  const auto &binds = m_config->keyBinds.bindings;
  auto toAction = [&]() -> ViewActions {
    if (key == binds.at(Action::MOVE_UP))
      return ViewActions::SCROLL_UP;
    if (key == binds.at(Action::MOVE_DOWN))
      return ViewActions::SCROLL_DOWN;
    if (key == binds.at(Action::VIEW_EDIT))
      return ViewActions::EDIT;
    if (key == binds.at(Action::ESCAPE))
      return ViewActions::BACK;
    return ViewActions::NONE;
  };

  switch (toAction()) {
    case ViewActions::SCROLL_UP:
      moveUp();
      return nullptr;
    case ViewActions::SCROLL_DOWN:
      moveDown();
      return nullptr;
    case ViewActions::EDIT: {
      m_note.content = Config::Editor::openEditor(m_note.content);
      auto result = m_repository.update(m_note);
      wclear(m_pad);
      m_widget->draw();
      return nullptr;
    };
    case ViewActions::BACK:
      m_controller.popState();
      return nullptr;
    case ViewActions::NONE:
      return nullptr;
  }
  return nullptr;
}

} // namespace QuickNotes::App::State
