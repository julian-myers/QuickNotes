#include "app/state/NewNoteState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "ncurses.h"
#include "ui/AddNoteWidget.hpp"
#include <cctype>
#include <expected>
#include <memory>
#include <vector>

namespace QuickNotes::App::State {

using namespace DB;

NewNoteState::NewNoteState(
    WINDOW *window,
    std::shared_ptr<const Config::Config> config,
    IAppController &controller,
    INotesRepository &repository,
    std::vector<Model::Note> &notes
) noexcept
    : NoteAwareState(window, config, controller, repository), m_notes(notes) {}

void NewNoteState::onEnter() {
  m_inputBuffer.clear();
  m_widget = std::make_unique<UI::AddNoteWidget>(m_window);
  curs_set(1);
}

void NewNoteState::onExit() {
  curs_set(0);
  m_widget.reset();
  wclear(m_window);
}

void NewNoteState::render() {
  m_widget->setInputBuffer(m_inputBuffer);
  m_widget->draw();
}

std::unique_ptr<AbstractState> NewNoteState::handleInput(int key) {
  switch (toAction(key)) {
    case NewNoteAction::CONFIRM: handleConfirm(); return nullptr;
    case NewNoteAction::CANCEL: m_controller.popState(); return nullptr;
    case NewNoteAction::BACKSPACE:
      m_widget->setError("");
      clearError();
      if (!m_inputBuffer.empty()) m_inputBuffer.pop_back();
      return nullptr;
    case NewNoteAction::APPEND:
      m_widget->setError("");
      clearError();
      if (std::isprint(key)) m_inputBuffer += static_cast<char>(key);
      return nullptr;
    case NewNoteAction::NONE: return nullptr;
  }
  return nullptr;
}

// ---- Private Methods ----

NewNoteState::NewNoteAction NewNoteState::toAction(int key) const {
  const auto &binds = m_config->keyBinds.bindings;
  using Predicate = std::function<bool()>;
  using Action = Config::Action;
  const std::vector<std::pair<Predicate, NewNoteAction>> dispatch = {
      {[&] { return key == binds.at(Action::ESCAPE); }, NewNoteAction::CANCEL},
      {[&] { return key == binds.at(Action::SELECT); }, NewNoteAction::CONFIRM},
      {[&] { return key == binds.at(Action::BACKSPACE); },
       NewNoteAction::BACKSPACE},
      {[&] { return std::isprint(key) != 0; }, NewNoteAction::APPEND},
  };
  auto it = std::ranges::find_if(dispatch, [&](const auto &entry) {
    return entry.first();
  });
  return (it != dispatch.end()) ? it->second : NewNoteAction::NONE;
}

void NewNoteState::handleConfirm() {
  using expected = std::expected<Model::Note, std::string>;
  if (m_inputBuffer.empty()) {
    m_controller.popState();
    return;
  }
  m_repository.create(m_inputBuffer)
      .and_then([&](Model::Note created) -> expected {
        m_notes.push_back(created);
        m_controller.popState();
        return created;
      })
      .or_else([&](const std::string &error) -> expected {
        setError(error);
        m_widget->setError(error);
        return std::unexpected(error);
      });
}

} // namespace QuickNotes::App::State
