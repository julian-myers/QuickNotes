#include "app/state/RenameNoteState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include <cctype>
#include <functional>
#include <memory>

namespace QuickNotes::App::State {

using namespace DB;

RenameNoteState::RenameNoteState(
    WINDOW *window,
    ConfigPtr config,
    IAppController &controller,
    INotesRepository &repository,
    std::vector<Model::Note> &notes,
    int selectedIndex
)
    : NoteAwareState(window, config, controller, repository),
      m_notes(notes),
      m_selectedIndex(selectedIndex) {}

void RenameNoteState::onEnter() {
  m_inputBuffer.clear();
  m_widget = std::make_unique<UI::RenameNoteWidget>(m_window);
  curs_set(1);
}

void RenameNoteState::onExit() {
  curs_set(0);
  m_widget.reset();
  wclear(m_window);
}

void RenameNoteState::render() {
  m_widget->setInputBuffer(m_inputBuffer);
  m_widget->draw();
}

std::unique_ptr<AbstractState> RenameNoteState::handleInput(int key) {
  switch (toAction(key)) {
    case RenameAction::CONFIRM: handleConfirm(); return nullptr;
    case RenameAction::CANCEL: m_controller.popState(); return nullptr;
    case RenameAction::BACKSPACE:
      clearError();
      m_widget->setError("");
      if (!m_inputBuffer.empty()) m_inputBuffer.pop_back();
      return nullptr;
    case RenameAction::APPEND:
      clearError();
      m_widget->setError("");
      if (std::isprint(key)) m_inputBuffer += static_cast<char>(key);
      return nullptr;
    case RenameAction::NONE: return nullptr;
  }
  return nullptr;
}

// ---- Private Methods ----

RenameNoteState::RenameAction RenameNoteState::toAction(int key) const {
  const auto &binds = m_config->keyBinds.bindings;
  using Action = Config::Action;
  const std::vector<std::pair<std::function<bool()>, RenameAction>> dispatch = {
      {[&] { return key == binds.at(Action::ESCAPE); }, RenameAction::CANCEL},
      {[&] { return key == binds.at(Action::SELECT); }, RenameAction::CONFIRM},
      {[&] { return key == binds.at(Action::BACKSPACE) || key == KEY_BACKSPACE; },
       RenameAction::BACKSPACE},
      {[&] { return std::isprint(key) != 0; }, RenameAction::APPEND},
  };
  auto it = std::ranges::find_if(dispatch, [](const auto &entry) {
    return entry.first(); // NOLINT
  });
  return (it != dispatch.end()) ? it->second : RenameAction::NONE;
}

void RenameNoteState::handleConfirm() {
  if (m_inputBuffer.empty()) {
    m_controller.popState();
    return;
  }
  Model::Note note = m_notes[m_selectedIndex];
  note.title = m_inputBuffer;
  [[maybe_unused]] auto result =
      m_repository.update(note)
          .and_then([&](Model::Note updated) -> INotesRepository::NoteResult {
            m_notes[m_selectedIndex] = updated;
            m_controller.popState();
            return updated;
          })
          .or_else([&](const std::string &error) -> INotesRepository::NoteResult {
            setError(error);
            m_widget->setError(error);
            return std::unexpected(error);
          });
}

} // namespace QuickNotes::App::State
