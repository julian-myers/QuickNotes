#pragma once

#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "config/Config.hpp"
#include "db/INoteRepository.hpp"
#include "ncurses.h"
#include <memory>

namespace QuickNotes::App::State {

class NoteAwareState : public AbstractState {
  public:
    NoteAwareState(
        WINDOW *window,
        std::shared_ptr<const Config::Config> config,
        IAppController &controller,
        DB::INotesRepository &repository
    )
        : AbstractState(window, config, controller), m_repository(repository) {}

  protected:
    DB::INotesRepository &m_repository;
    std::string m_errorMessage;

    void setError(const std::string &message) { m_errorMessage = message; }
    void clearError() { m_errorMessage.clear(); }
};

} // namespace QuickNotes::App::State
