#include "app/state/NoteListState.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/DeleteNoteState.hpp"
#include "app/state/NewNoteState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "app/state/ViewingState.hpp"
#include "config/Config.hpp"
#include "config/Editor.hpp"
#include "db/NoteRepository.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/PreviewWidget.hpp"
#include <algorithm>
#include <memory>
#include <stdexcept>

namespace QuickNotes::App::State {
using ConfigPtr = std::shared_ptr<const Config::Config>;

NoteListState::NoteListState(
    WINDOW *window,
    ConfigPtr config,
    IAppController &controller,
    DB::NotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository),
      m_selectedIndex(0), m_listWindow(nullptr), m_previewWindow(nullptr) {}

void NoteListState::onEnter() {
  loadNotes();
  createWindow();
}

void NoteListState::onExit() {
  m_list.reset();
  m_preview.reset();
  if (m_listWindow) {
    delwin(m_listWindow);
    m_listWindow = nullptr;
  }
  if (m_previewWindow) {
    delwin(m_previewWindow);
    m_previewWindow = nullptr;
  }
  wclear(m_window);
}

const std::vector<NoteListState::Binding> NoteListState::m_keyMap{
    {Config::Action::MOVE_UP, ListActions::MOVE_UP},
    {Config::Action::MOVE_DOWN, ListActions::MOVE_DOWN},
    {Config::Action::MOVE_RIGHT, ListActions::VIEW_NOTE},
    {Config::Action::SELECT, ListActions::SELECT},
    {Config::Action::SEARCH, ListActions::SEARCH},
    {Config::Action::NEW_NOTE, ListActions::NEW_NOTE},
    {Config::Action::DELETE_NOTE, ListActions::DELETE_NOTE},
    {Config::Action::QUIT, ListActions::QUIT},
};

std::unique_ptr<AbstractState> NoteListState::handleInput(int key) {
  const auto &binds = m_config->keyBinds.bindings;
  auto it = std::ranges::find_if(m_keyMap, [&](const auto &pair) {
    return key == binds.at(pair.first);
  });
  auto action = (it != m_keyMap.end()) ? it->second : ListActions::NONE;
  switch (action) {
    case ListActions::MOVE_UP:
      moveUp();
      return nullptr;
    case ListActions::MOVE_DOWN:
      moveDown();
      return nullptr;
    case ListActions::VIEW_NOTE:
      return std::make_unique<ViewingState>(
          m_window,
          m_config,
          m_controller,
          m_repository,
          m_notes[m_selectedIndex]
      );
    case ListActions::SELECT:
      {
        Model::Note &note = m_notes[m_selectedIndex];
        note.content = Config::Editor::openEditor(note.content);
        auto result = m_repository.update(note);
        return nullptr;
      };
    case ListActions::SEARCH:
      return nullptr;
    case ListActions::NEW_NOTE:
      return std::make_unique<NewNoteState>(
          m_window, m_config, m_controller, m_repository, m_notes
      );
    case ListActions::DELETE_NOTE:
      return std::make_unique<DeleteNoteState>(
          m_window,
          m_config,
          m_controller,
          m_repository,
          m_notes,
          m_selectedIndex
      );
    case ListActions::QUIT:
      m_controller.quit();
      return nullptr;
    case ListActions::NONE:
      return nullptr;
  }
  return nullptr;
}

void NoteListState::render() {
  wclear(m_window);
  wrefresh(m_window);
  m_list->setSelectedIndex(m_selectedIndex);
  m_list->draw(m_notes);
  if (!m_notes.empty()) {
    m_preview->draw(m_notes[m_selectedIndex]);
  }
  wrefresh(m_window);
}

void NoteListState::moveUp() {
  m_selectedIndex = std::max(0, m_selectedIndex - 1);
}

void NoteListState::moveDown() {
  if (m_notes.empty()) return;
  m_selectedIndex =
      std::min(static_cast<int>(m_notes.size()) - 1, m_selectedIndex + 1);
}

// ----- Private Methods -----

void NoteListState::createWindow() {
  wclear(m_window);
  int height = getmaxy(m_window);
  int width = getmaxx(m_window);
  int listWidth = width / 3;
  int previewWidth = width - listWidth;
  int listStart = 0;
  int margin = 2;
  m_listWindow = derwin(m_window, height, listWidth, listStart, listStart);
  m_previewWindow = derwin(
      m_window,
      height - (margin * 2),
      previewWidth - (margin * 2),
      listStart + margin,
      listWidth + margin
  );
  m_list = std::make_unique<UI::NoteListWidget>(m_listWindow);
  m_preview = std::make_unique<UI::PreviewWidget>(m_previewWindow, m_config);
}

void NoteListState::loadNotes() {
  try {
    m_notes = m_repository.findAll();
  } catch (const std::runtime_error &e) {
    m_notes = {};
  }
}

} // namespace QuickNotes::App::State
