#include "app/state/NoteListState.hpp"
#include "StateUtils.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/DeleteNoteState.hpp"
#include "app/state/NewNoteState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "app/state/RenameNoteState.hpp"
#include "config/Config.hpp"
#include "config/Editor.hpp"
#include "db/INoteRepository.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/NoteContainer.hpp"
#include <algorithm>
#include <cctype>
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
      m_selectedIndex(0),
      m_view(nullptr) {}

void NoteListState::onEnter() {
  wclear(m_window);
  loadNotes();
  m_view = std::make_unique<UI::NoteContainer>(m_window, m_config);
}

void NoteListState::onExit() {
  m_view.reset();
  wclear(m_window);
}

void NoteListState::render() {
  m_view->setVisual(m_mode == Mode::VISUAL);
  const std::string label = m_errorMessage.empty() ? std::string(modeLabel()) :
                                                     "ERR: " + m_errorMessage;

  switch (m_mode) {
    case Mode::NORMAL:
      m_view->setMode(label);
      if (m_tagging) {
        m_view->setSearchTyping(true);
        m_view->draw(m_notes, m_selectedIndex, m_inputBuffer);
      } else {
        m_view->draw(m_notes, m_selectedIndex);
      }
      m_lastStateWasSearch = false;
      break;
    case Mode::SEARCH:
      m_view->setMode(label);
      m_view->setSearchTyping(m_searchTyping);
      m_view->draw(m_results, m_selectedIndex, m_query);
      m_lastStateWasSearch = true;
      break;
    case Mode::PREVIEW:
      if (m_lastStateWasSearch) {
        m_view->setMode(label);
        m_view->draw(m_results, m_selectedIndex, m_query);
        return;
      }
      m_view->setMode(label);
      m_view->draw(m_notes, m_selectedIndex);
      break;
    case Mode::VISUAL:
      m_view->setMode(label);
      if (m_tagging) {
        m_view->setSearchTyping(true);
        m_view->draw(m_notes, m_selectedIndex, m_inputBuffer);
      } else {
        m_view->draw(m_notes, m_selectedIndex, m_visualStart);
      }
      m_lastStateWasSearch = false;
      break;
  }
}

std::unique_ptr<AbstractState> NoteListState::handleInput(int key) {
  clearError();
  switch (m_mode) {
    case Mode::NORMAL: return handleNormal(key);
    case Mode::SEARCH: return handleSearch(key);
    case Mode::PREVIEW: return handlePreview(key);
    case Mode::VISUAL: return handleVisual(key);
  }
  return nullptr;
}

// ----- Private Methods -----

const std::vector<NoteListState::NormalBinding> NoteListState::m_normalKeyMap{
    {Config::Action::MOVE_UP, NormalAction::MOVE_UP},
    {Config::Action::MOVE_DOWN, NormalAction::MOVE_DOWN},
    {Config::Action::MOVE_RIGHT, NormalAction::FOCUS_PREVIEW},
    {Config::Action::SELECT, NormalAction::EDIT_NOTE},
    {Config::Action::SEARCH, NormalAction::SEARCH},
    {Config::Action::NEW_NOTE, NormalAction::NEW_NOTE},
    {Config::Action::DELETE_NOTE, NormalAction::DELETE_NOTE},
    {Config::Action::RENAME_NOTE, NormalAction::RENAME_NOTE},
    {Config::Action::PIN_NOTE, NormalAction::PIN_NOTE},
    {Config::Action::TAG_NOTE, NormalAction::TAG_NOTE},
    {Config::Action::VISUAL_MODE, NormalAction::VISUAL},
    {Config::Action::QUIT, NormalAction::QUIT},
};

const std::vector<NoteListState::VisualBinding> NoteListState::m_visualKeyMap{
    {Config::Action::MOVE_UP, VisualAction::MOVE_UP},
    {Config::Action::MOVE_DOWN, VisualAction::MOVE_DOWN},
    {Config::Action::PIN_NOTE, VisualAction::PIN},
    {Config::Action::TAG_NOTE, VisualAction::ADD_TAG},
};

std::unique_ptr<AbstractState> NoteListState::handleNormal(int key) {
  const auto &binds = m_config->keyBinds.bindings;

  if (m_tagging) {
    if (key == binds.at(Config::Action::ESCAPE)) {
      m_tagging = false;
      m_inputBuffer.clear();
      return nullptr;
    }
    if (key == binds.at(Config::Action::SELECT)) {
      if (!m_inputBuffer.empty() && !m_notes.empty()) {
        m_repository.addTag(m_notes[m_selectedIndex].id, m_inputBuffer);
        loadNotes();
      }
      m_tagging = false;
      m_inputBuffer.clear();
      return nullptr;
    }
    if (key == binds.at(Config::Action::BACKSPACE) || key == KEY_BACKSPACE) {
      if (!m_inputBuffer.empty()) m_inputBuffer.pop_back();
      return nullptr;
    }
    if (std::isprint(static_cast<unsigned char>(key))) {
      m_inputBuffer += static_cast<char>(key);
    }
    return nullptr;
  }

  const auto action =
      dispatchKey(key, m_config->keyBinds, m_normalKeyMap, NormalAction::NONE);
  switch (action) {
    case NormalAction::MOVE_UP: moveUp(); return nullptr;
    case NormalAction::MOVE_DOWN: moveDown(); return nullptr;
    case NormalAction::FOCUS_PREVIEW: m_mode = Mode::PREVIEW; return nullptr;
    case NormalAction::EDIT_NOTE:
      {
        if (m_notes.empty()) return nullptr;
        Model::Note &note = m_notes[m_selectedIndex];
        note.content = Config::Editor::openEditor(note.content);
        auto result = m_repository.update(note);
        if (!result) setError(result.error());
        m_view->resetScroll();
        return nullptr;
      };
    case NormalAction::SEARCH: enterSearchMode(); return nullptr;
    case NormalAction::NEW_NOTE:
      return makeState<NewNoteState>(m_repository, m_notes);
    case NormalAction::DELETE_NOTE:
      return makeState<DeleteNoteState>(m_repository, m_notes, m_selectedIndex);
    case NormalAction::RENAME_NOTE:
      return makeState<RenameNoteState>(m_repository, m_notes, m_selectedIndex);
    case NormalAction::PIN_NOTE:
      {
        if (m_notes.empty()) return nullptr;
        const Model::Note &note = m_notes[m_selectedIndex];
        m_repository.setPinned(note.id, !note.pinned);
        loadNotes();
        return nullptr;
      }
    case NormalAction::TAG_NOTE:
      {
        if (m_notes.empty()) return nullptr;
        m_tagging = true;
        m_tagFromVisual = false;
        m_inputBuffer.clear();
        return nullptr;
      }
    case NormalAction::VISUAL:
      enterVisualMode();
      return nullptr;
    case NormalAction::QUIT: m_controller.quit(); return nullptr;
    case NormalAction::NONE: return nullptr;
  }
  return nullptr;
}

std::unique_ptr<AbstractState> NoteListState::handleSearch(int key) {
  const auto &binds = m_config->keyBinds.bindings;

  if (m_searchTyping) {
    if (key == binds.at(Config::Action::ESCAPE) ||
        key == binds.at(Config::Action::SEARCH)) {
      if (!m_query.empty()) {
        m_query.clear();
        m_results = m_notes;
        m_selectedIndex = 0;
        m_searchTyping = false;
      } else {
        exitSearchMode();
      }
      return nullptr;
    }
    if (key == binds.at(Config::Action::SELECT) ||
        key == binds.at(Config::Action::MOVE_RIGHT)) {
      m_searchTyping = false;
      return nullptr;
    }
    if (key == binds.at(Config::Action::BACKSPACE) || key == KEY_BACKSPACE) {
      if (!m_query.empty()) {
        m_query.pop_back();
        updateSearch();
      }
      return nullptr;
    }
    if (std::isprint(static_cast<unsigned char>(key))) {
      m_query += static_cast<char>(key);
      updateSearch();
    }
  } else {
    // NAVIGATING: j/k move, ESC exits, '/' restarts typing
    if (key == binds.at(Config::Action::ESCAPE)) {
      exitSearchMode();
      return nullptr;
    }
    if (key == binds.at(Config::Action::SEARCH)) {
      m_query.clear();
      m_results = m_notes;
      m_selectedIndex = 0;
      m_searchTyping = true;
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
      if (!m_results.empty()) m_mode = Mode::PREVIEW;
      return nullptr;
    }
  }
  return nullptr;
}

std::unique_ptr<AbstractState> NoteListState::handlePreview(int key) {
  const auto &binds = m_config->keyBinds.bindings;
  if (key == binds.at(Config::Action::MOVE_UP)) {
    m_view->scrollUp();
    return nullptr;
  }
  if (key == binds.at(Config::Action::MOVE_DOWN)) {
    m_view->scrollDown();
    return nullptr;
  }
  if (key == binds.at(Config::Action::MOVE_LEFT)) {
    m_mode = Mode::NORMAL;
    return nullptr;
  }
  return nullptr;
}

std::unique_ptr<AbstractState> NoteListState::handleVisual(int key) {
  const auto &binds = m_config->keyBinds.bindings;

  if (m_tagging) {
    if (key == binds.at(Config::Action::ESCAPE)) {
      m_tagging = false;
      m_inputBuffer.clear();
      return nullptr;
    }
    if (key == binds.at(Config::Action::SELECT)) {
      if (!m_inputBuffer.empty() && !m_notes.empty()) {
        const int lo = std::min(m_selectedIndex, m_visualStart);
        const int hi = std::max(m_selectedIndex, m_visualStart);
        for (int i = lo; i <= hi && i < static_cast<int>(m_notes.size()); ++i) {
          m_repository.addTag(m_notes[i].id, m_inputBuffer);
        }
        loadNotes();
      }
      m_tagging = false;
      m_inputBuffer.clear();
      exitVisualMode();
      return nullptr;
    }
    if (key == binds.at(Config::Action::BACKSPACE) || key == KEY_BACKSPACE) {
      if (!m_inputBuffer.empty()) m_inputBuffer.pop_back();
      return nullptr;
    }
    if (std::isprint(static_cast<unsigned char>(key))) {
      m_inputBuffer += static_cast<char>(key);
    }
    return nullptr;
  }

  if (key == binds.at(Config::Action::ESCAPE)) {
    exitVisualMode();
    return nullptr;
  }

  const auto action =
      dispatchKey(key, m_config->keyBinds, m_visualKeyMap, VisualAction::NONE);
  switch (action) {
    case VisualAction::MOVE_UP: moveUp(); return nullptr;
    case VisualAction::MOVE_DOWN: moveDown(); return nullptr;
    case VisualAction::PIN:
      {
        if (m_notes.empty()) return nullptr;
        const int lo = std::min(m_selectedIndex, m_visualStart);
        const int hi = std::max(m_selectedIndex, m_visualStart);
        bool anyUnpinned = false;
        for (int i = lo; i <= hi && i < static_cast<int>(m_notes.size()); ++i) {
          if (!m_notes[i].pinned) { anyUnpinned = true; break; }
        }
        for (int i = lo; i <= hi && i < static_cast<int>(m_notes.size()); ++i) {
          m_repository.setPinned(m_notes[i].id, anyUnpinned);
        }
        loadNotes();
        exitVisualMode();
        return nullptr;
      }
    case VisualAction::ADD_TAG:
      {
        if (m_notes.empty()) return nullptr;
        m_tagging = true;
        m_tagFromVisual = true;
        m_inputBuffer.clear();
        return nullptr;
      }
    case VisualAction::NONE: return nullptr;
  }
  return nullptr;
}

void NoteListState::enterVisualMode() {
  m_mode = Mode::VISUAL;
  m_visualStart = m_selectedIndex;
}

void NoteListState::exitVisualMode() {
  m_mode = Mode::NORMAL;
  m_tagging = false;
  m_tagFromVisual = false;
  m_inputBuffer.clear();
}

void NoteListState::moveUp() {
  m_selectedIndex = std::max(0, m_selectedIndex - 1);
  m_view->resetScroll();
}

void NoteListState::moveDown() {
  const auto &list = activeNotes();
  if (list.empty()) return;
  m_selectedIndex =
      std::min(static_cast<int>(list.size()) - 1, m_selectedIndex + 1);
  m_view->resetScroll();
}

void NoteListState::enterSearchMode() {
  m_mode = Mode::SEARCH;
  m_query.clear();
  m_results = m_notes;
  m_selectedIndex = 0;
  m_searchTyping = true;
}

void NoteListState::exitSearchMode() {
  m_mode = Mode::NORMAL;
  m_query.clear();
  m_results.clear();
  m_selectedIndex = 0;
  m_searchTyping = false;
}

void NoteListState::updateSearch() {
  try {
    m_results = m_query.empty() ? m_notes : m_repository.findByTitle(m_query);
  } catch (const std::runtime_error &e) {
    m_results = {};
    setError(e.what());
  }
  m_selectedIndex = 0;
}

std::vector<Model::Note> &NoteListState::activeNotes() {
  return (m_mode == Mode::SEARCH) ? m_results : m_notes;
}

std::string_view NoteListState::modeLabel() const {
  if (m_tagging) return "--- TAG ---";
  switch (m_mode) {
    case Mode::NORMAL:  return "--- NORMAL ---";
    case Mode::SEARCH:  return m_searchTyping ? "--- SEARCH ---" : "--- NAVIGATE ---";
    case Mode::PREVIEW: return "--- PREVIEW ---";
    case Mode::VISUAL:  return "--- VISUAL ---";
  }
  return "--- NORMAL ---";
}

void NoteListState::loadNotes() {
  try {
    m_notes = m_repository.findAll();
    std::stable_sort(
        m_notes.begin(),
        m_notes.end(),
        [](const Model::Note &a, const Model::Note &b) {
          return a.pinned > b.pinned;
        }
    );
  } catch (const std::runtime_error &e) {
    m_notes = {};
    setError(e.what());
  }
}

} // namespace QuickNotes::App::State
