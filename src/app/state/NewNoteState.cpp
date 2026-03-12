#include "app/state/NewNoteState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "db/NoteRepository.hpp"
#include "ncurses.h"
#include "ui/AddNoteWidget.hpp"
#include <expected>
#include <memory>
#include <vector>

namespace QuickNotes::App::State {

using namespace DB;

NewNoteState::NewNoteState(
    WINDOW *window,
    std::shared_ptr<Config::Config> config,
    IAppController &controller,
    NotesRepository &repository,
    std::vector<Model::Note> &notes
) noexcept
    : NoteAwareState(window, config, controller, repository), m_notes(notes),
      m_addNoteWidget(std::make_unique<UI::AddNoteWidget>(window)) {}

void NewNoteState::onEnter() { m_title = m_addNoteWidget->getInput(); }

void NewNoteState::onExit() { m_addNoteWidget.reset(); }

void NewNoteState::render() {}

std::unique_ptr<AbstractState> NewNoteState::handleInput(int key) {
  using Action = Config::Action;
  const auto &binds = m_config->keyBinds.bindings;
  auto toAction = [&]() -> NewNoteAction {
    if (key == binds.at(Action::ESCAPE)) return NewNoteAction::CANCEL;
    if (key == binds.at(Action::SELECT)) return NewNoteAction::CONFIRM;
    return NewNoteAction::NONE;
  };
  switch (toAction()) {
    case NewNoteAction::CONFIRM:
      handleConfirm();
      return nullptr;
    case NewNoteAction::CANCEL:
      m_controller.popState();
      return nullptr;
    case NewNoteAction::NONE:
      return nullptr;
  }
}

// ---- Private Methods ----

void NewNoteState::handleConfirm() {
  using expected = std::expected<Model::Note, std::string>;
  auto result = m_repository.create(m_title)
                    .and_then([&](Model::Note created) -> expected {
                      m_notes.push_back(created);
                      return created;
                    })
                    .or_else([&](const std::string &error) -> expected {
                      setError(error);
                      return std::unexpected(error);
                    });
  m_controller.popState();
}

} // namespace QuickNotes::App::State
