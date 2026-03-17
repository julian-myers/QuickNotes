#include "DeleteNoteState.hpp"
#include "StateUtils.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/DeleteConfirmWidget.hpp"
#include <algorithm>
#include <vector>

namespace QuickNotes::App::State {
using ConfigPtr = std::shared_ptr<const Config::Config>;

DeleteNoteState::DeleteNoteState(
    WINDOW *window,
    ConfigPtr config,
    IAppController &controller,
    DB::INotesRepository &repository,
    std::vector<Model::Note> &notes,
    int selectedIndex
)
    : NoteAwareState(window, config, controller, repository), m_notes(notes),
      m_note(m_notes[selectedIndex]), m_selectedIndex(selectedIndex) {}

void DeleteNoteState::onEnter() {
  m_widget = std::make_unique<UI::DeleteConfirmWidget>(m_window);
}
void DeleteNoteState::onExit() {}
void DeleteNoteState::render() { m_widget->draw(); }

const std::vector<DeleteNoteState::Binding> DeleteNoteState::m_keyMap{
    {Config::Action::NO, DeleteConfirmAction::CANCEL},
    {Config::Action::YES, DeleteConfirmAction::CONFIRM},
};

std::unique_ptr<AbstractState> DeleteNoteState::handleInput(int key) {
  auto action =
      dispatchKey(key, m_config->keyBinds, m_keyMap, DeleteConfirmAction::NONE);
  switch (action) {
    case DeleteConfirmAction::CONFIRM:
      handleConfirm();
      m_controller.popState();
      return nullptr;
    case DeleteConfirmAction::CANCEL: m_controller.popState(); return nullptr;
    case DeleteConfirmAction::NONE: return nullptr;
  }
}

void DeleteNoteState::handleConfirm() {
  using expected = std::expected<Model::Note, std::string>;
  auto result =
      m_repository.remove(m_note)
          .and_then([&](Model::Note note) -> expected {
            m_notes.erase(m_notes.begin() + m_selectedIndex);
            m_selectedIndex =
                std::min(m_selectedIndex, static_cast<int>(m_notes.size() - 1));
            return m_note;
          })
          .or_else([&](const std::string &error) -> expected {
            setError(error);
            return std::unexpected(error);
          });
}
} // namespace QuickNotes::App::State
