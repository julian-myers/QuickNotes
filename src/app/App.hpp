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

/// @brief Contains the main loop and wires all the processes together.
///
/// Has members m_state and m_config.
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

    void pushState(std::unique_ptr<AbstractState> state) override;
    void popState() override;

  private:
    std::shared_ptr<Config::Config> m_config;
    DB::Database m_db;
    DB::NotesRepository m_repository;
    std::stack<std::unique_ptr<AbstractState>> m_stateStack;
    bool m_running = true;
};
} // namespace QuickNotes::App
