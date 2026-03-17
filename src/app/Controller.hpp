#pragma once

#include <memory>

namespace QuickNotes::App::State {
class AbstractState;
}

namespace QuickNotes::App {

/// @brief Interface that states use to drive application-level transitions.
///
/// Passed by reference into every AbstractState so that states can request
/// lifecycle operations (quit, push a new state, pop themselves) without
/// holding a direct reference to App. This keeps the state classes decoupled
/// from the concrete application class.
///
/// @see App
/// @see AbstractState
class IAppController {
  public:
    virtual ~IAppController() = default;

    /// @brief Signal the application to exit its main loop.
    virtual void quit() = 0;

    /// @brief Push a new state onto the state stack, transitioning to it.
    /// @param state The state to transition to.
    virtual void pushState(std::unique_ptr<State::AbstractState> state) = 0;

    /// @brief Pop the current state from the stack, returning to the previous one.
    virtual void popState() = 0;
};

} // namespace QuickNotes::App
