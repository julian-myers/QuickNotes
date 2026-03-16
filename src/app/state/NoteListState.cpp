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

namespace QuickNotes::App::State {
using ConfigPtr = std::shared_ptr<const Config::Config>;

NoteListState::NoteListState(
    WINDOW *window,
    ConfigPtr config,
    IAppController &controller,
    DB::INotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository),
      m_selectedIndex(0), m_view(nullptr) {}

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
  return m_mode == Mode::NORMAL ? handleNormal(key) : handleSearch(key);
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
    case NormalAction::SEARCH: enterSearchMode(); return nullptr;
    case NormalAction::NEW_NOTE:
      {
        return std::make_unique<NewNoteState>(
            m_window, m_config, m_controller, m_repository, m_notes
        );
      }
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

std::unique_ptr<AbstractState> NoteListState::handleSearch(int key) {
  const auto &binds = m_config->keyBinds.bindings;

  if (key == binds.at(Config::Action::ESCAPE)) {
    exitSearchMode();
    return nullptr;
  }

  if (key == binds.at(Config::Action::MOVE_UP)) {
    moveUp();
    return nullptr;
  }
  if (key == binds.at(Config::Action::MOVE_DOWN)) {
    moveDown();
    return nullptr;
  }

  if (key == binds.at(Config::Action::SELECT) ||
      key == binds.at(Config::Action::MOVE_RIGHT)) {
    if (m_results.empty()) return nullptr;
    return std::make_unique<ViewingState>(
        m_window,
        m_config,
        m_controller,
        m_repository,
        m_results[m_selectedIndex]
    );
  }

  if (key == KEY_BACKSPACE && !m_query.empty()) {
    m_query.pop_back();
    updateSearch();
    return nullptr;
  }

  if (std::isprint(static_cast<unsigned char>(key))) {
    m_query += static_cast<char>(key);
    updateSearch();
  }

  return nullptr;
}

void NoteListState::render() {
  switch (m_mode) {
    case Mode::NORMAL:
      m_view->setMode("--- NORMAL ---");
      m_view->draw(m_notes, m_selectedIndex);
      break;
    case Mode::SEARCH:
      m_view->setMode("--- SEARCH ----");
      m_view->draw(m_results, m_selectedIndex, m_query);
      break;
    default: break;
  }
}

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
  const auto &list = activeNotes();
  if (list.empty()) return;
  m_selectedIndex =
      std::min(static_cast<int>(m_notes.size()) - 1, m_selectedIndex + 1);
}

void NoteListState::enterSearchMode() {
  m_mode = Mode::SEARCH;
  m_query.clear();
  m_results = m_notes;
  m_selectedIndex = 0;
}

void NoteListState::exitSearchMode() {
  m_mode = Mode::NORMAL;
  m_query.clear();
  m_results.clear();
  m_selectedIndex = 0;
}

void NoteListState::updateSearch() {
  try {
    m_results = m_query.empty() ? m_notes : m_repository.findByTitle(m_query);
  } catch (const std::runtime_error &e) {
    m_results = {};
  }
  m_selectedIndex = 0;
}

std::vector<Model::Note> &NoteListState::activeNotes() {
  return m_mode == Mode::NORMAL ? m_notes : m_results;
}

void NoteListState::loadNotes() {
  try {
    m_notes = m_repository.findAll();
  } catch (const std::runtime_error &e) {
    m_notes = {};
    setError(e.what());
  }
}

} // namespace QuickNotes::App::State
