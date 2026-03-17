#pragma once

#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "config/Config.hpp"
#include "db/INoteRepository.hpp"
#include "ncurses.h"
#include <memory>

namespace QuickNotes::App::State {

/// @brief Intermediate base for states that need access to the notes repository.
///
/// Extends AbstractState with a reference to INotesRepository and a simple
/// error-message slot. States that create, delete, or query notes should
/// inherit from this class instead of AbstractState directly.
///
/// @see AbstractState
/// @see DB::INotesRepository
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

    /// @brief Human-readable error set when a repository operation fails.
    std::string m_errorMessage;

    /// @brief Store an error message to be surfaced in the next render pass.
    /// @param message The error string returned by the repository.
    void setError(const std::string &message) { m_errorMessage = message; }

    /// @brief Clear any previously stored error message.
    void clearError() { m_errorMessage.clear(); }
};

} // namespace QuickNotes::App::State
