#pragma once

#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "config/Config.hpp"
#include "db/NoteRepository.hpp"
#include "ncurses.h"
#include <concepts>
#include <memory>

namespace QuickNotes::App::State {

class NoteAwareState : public AbstractState {
  public:
    NoteAwareState(
        WINDOW *window,
        std::shared_ptr<Config::Config> config,
        IAppController &controller,
        DB::NotesRepository &repository
    )
        : AbstractState(window, config, controller), m_repository(repository) {}

  protected:
    template <typename T>
      requires std::derived_from<T, NoteAwareState>
    std::unique_ptr<T> makeNoteAwareState() {
      return std::make_unique<T>(
          m_window, m_config, m_controller, m_repository
      );
    }

    void setError(const std::string &message) { m_errorMessage = message; }
    void clearError() { m_errorMessage.clear(); }

    DB::NotesRepository &m_repository;
    std::string m_errorMessage;
};

} // namespace QuickNotes::App::State
