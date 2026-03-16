#include "SearchingState.hpp"
#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "db/INoteRepository.hpp"
#include <cctype>
#include <memory>

namespace QuickNotes::App::State {

SearchingState::SearchingState(
    WINDOW *window,
    ConfigPtr config,
    IAppController &controller,
    DB::INotesRepository &repository
) noexcept
    : NoteAwareState(window, config, controller, repository) {}

void SearchingState::render() {}

void SearchingState::onEnter() { m_inputBuffer.clear(); }

void SearchingState::onExit() {}

std::unique_ptr<AbstractState> SearchingState::handleInput(int key) {
  if (key == m_config->keyBinds.bindings.at(Config::Action::ESCAPE)) {
    m_controller.popState();
    return nullptr;
  }
  if (std::isprint(key)) m_inputBuffer += static_cast<char>(key);
  return nullptr;
}

} // namespace QuickNotes::App::State
