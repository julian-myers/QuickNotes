#include "app/state/NoteListState.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/DeleteNoteState.hpp"
#include "app/state/NewNoteState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "app/state/ViewingState.hpp"
#include "config/Config.hpp"
#include "config/Editor.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/NoteContainer.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string_view>

namespace QuickNotes::App::State {
using ConfigPtr = std::shared_ptr<const Config::Config>;

NoteListState::NoteListState(
    WINDOW *window,
    ConfigPtr config,
    IAppController &controller,
    DB::INotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository),
      m_selectedIndex(0), m_view(nullptr), m_mode(Mode::NORMAL) {}

void NoteListState::onEnter() {
  wclear(m_window);
  loadNotes();
  m_view = std::make_unique<UI::NoteContainer>(m_window, m_config);
}

void NoteListState::onExit() {
  m_view.reset();
  wclear(m_window);
}

std::unique_ptr<AbstractState> NoteListState::handleInput(int key) {
  switch (m_mode) {
    case Mode::NORMAL: m_view->setMode(modeLabel()); return handleNormal(key);
    case Mode::SEARCH: m_view->setMode(modeLabel()); return handleSearch(key);
    case Mode::EDIT: m_view->setMode(modeLabel()); return handleEdit(key);
  }
}

void NoteListState::render() { m_view->draw(m_notes, m_selectedIndex); }

// ----- Private Methods -----

const std::vector<NoteListState::Binding> NoteListState::m_keyMap{
    {Config::Action::MOVE_UP, NormalAction::MOVE_UP},
    {Config::Action::MOVE_DOWN, NormalAction::MOVE_DOWN},
    {Config::Action::MOVE_RIGHT, NormalAction::VIEW_NOTE},
    {Config::Action::SELECT, NormalAction::SELECT},
    {Config::Action::SEARCH, NormalAction::SEARCH},
    {Config::Action::NEW_NOTE, NormalAction::NEW_NOTE},
    {Config::Action::DELETE_NOTE, NormalAction::DELETE_NOTE},
    {Config::Action::QUIT, NormalAction::QUIT},
};

void NoteListState::moveUp() {
  m_selectedIndex = std::max(0, m_selectedIndex - 1);
}

void NoteListState::moveDown() {
  if (m_notes.empty()) return;
  m_selectedIndex =
      std::min(static_cast<int>(m_notes.size()) - 1, m_selectedIndex + 1);
}

std::unique_ptr<AbstractState> NoteListState::handleNormal(int key) {
  const auto &binds = m_config->keyBinds.bindings;
  auto it = std::ranges::find_if(m_keyMap, [&](const auto &pair) {
    return key == binds.at(pair.first);
  });
  auto action = (it != m_keyMap.end()) ? it->second : NormalAction::NONE;
  switch (action) {
    case NormalAction::MOVE_UP: moveUp(); return nullptr;
    case NormalAction::MOVE_DOWN: moveDown(); return nullptr;
    case NormalAction::VIEW_NOTE:
      return std::make_unique<ViewingState>(
          m_window,
          m_config,
          m_controller,
          m_repository,
          m_notes[m_selectedIndex]
      );
    case NormalAction::SELECT:
      {
        Model::Note &note = m_notes[m_selectedIndex];
        note.content = Config::Editor::openEditor(note.content);
        auto result = m_repository.update(note);
        return nullptr;
      };
    case NormalAction::SEARCH: handleInput(key); return nullptr;
    case NormalAction::NEW_NOTE:
      return std::make_unique<NewNoteState>(
          m_window, m_config, m_controller, m_repository, m_notes
      );
    case NormalAction::DELETE_NOTE:
      return std::make_unique<DeleteNoteState>(
          m_window,
          m_config,
          m_controller,
          m_repository,
          m_notes,
          m_selectedIndex
      );
    case NormalAction::QUIT: m_controller.quit(); return nullptr;
    case NormalAction::NONE: return nullptr;
  }
  return nullptr;
}

// TODO: write this.
std::unique_ptr<AbstractState> NoteListState::handleSearch(int key) {
  return nullptr;
}

// TODO: write this.
std::unique_ptr<AbstractState> NoteListState::handleEdit(int key) {
  return nullptr;
}

void NoteListState::loadNotes() {
  try {
    m_notes = m_repository.findAll();
  } catch (const std::runtime_error &e) {
    m_notes = {};
  }
}

std::string_view NoteListState::modeLabel() const {
  using enum Mode;
  switch (m_mode) {
    case NORMAL: return "--- NORMAL ---";
    case SEARCH: return "--- SEARCH ---";
    case EDIT: return "--- EDIT ---";
  }
}

} // namespace QuickNotes::App::State
