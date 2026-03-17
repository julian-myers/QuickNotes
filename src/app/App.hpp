#pragma once

#include "app/Controller.hpp"
#include "config/Config.hpp"
#include "db/Database.hpp"
#include "db/NoteRepository.hpp"
#include "state/AbstractState.hpp"
#include <memory>
#include <stack>

namespace QuickNotes::App {
using namespace QuickNotes::App::State;

/// @brief Entry point and owner of all top-level application resources.
///
/// Initializes ncurses and the logger, loads configuration, opens the database,
/// and runs the main event loop. The loop drives the state machine by calling
/// render() and handleInput() on the top of m_stateStack each frame.
///
/// @see AbstractState
/// @see Config::Config
class App : public IAppController {

  public:
    /// @brief Constructs instance of App.
    App();

    /// @brief deconstructor.
    ~App() noexcept;

    /// @brief Main loop for the app.
    void run();

    /// @brief quit the app.
    void quit() override { m_running = false; }

    /// @brief Push a state onto the stack.
    ///
    /// Calls the onEnter() method of the state that is being pushed onto the
    /// stack.
    ///
    /// @param state The next state that is being transitioned to.
    void pushState(std::unique_ptr<AbstractState> state) override;

    /// @brief Pop the top state from the stack.
    ///
    /// Calls the top state's onExit() then pops it from the stack. If the stack
    /// is not empty then it will call the new top's render() method.
    void popState() override;

  private:
    std::shared_ptr<Config::Config> m_config;
    DB::Database m_db;
    DB::NotesRepository m_repository;
    std::stack<std::unique_ptr<AbstractState>> m_stateStack;
    bool m_running = true;

    /// @brief Initializes the logger.
    void setupLogger();

    /// @brief Sets ncurses parameters such as keypad, colors, default
    /// background, etc.
    void setupCurses();
};
} // namespace QuickNotes::App
