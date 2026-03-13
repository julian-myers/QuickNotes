#pragma once

#include "app/Controller.hpp"
#include "config/Config.hpp"
#include <memory>
#include <ncurses.h>

namespace QuickNotes::App::State {
class AbstractState {
  public:
    /// @brief Deconstructor
    virtual ~AbstractState() = default;

    /// @brief Called once when the state is initialized/ transitioned to.
    ///
    /// Override to perform any setup that should happen on entry, besides
    /// clearing the window.
    virtual void onEnter() = 0;

    /// @brief Called to tear down state to prepare for transition.
    ///
    /// Override in subclasses if just clearing the window is insufficient.
    virtual void onExit() = 0;

    /// @brief Renders the current state to screen.
    ///
    /// Called once per frame by the application loop. Override to draw whatever
    /// is appropriate for the current state. Implementations should call
    /// wrefresh() when done.
    virtual void render() = 0;

    /// @brief Handle the input from the user.
    ///
    /// Determines wether to transition to another state or adjust current
    /// state. Must be overridden per subclass.
    ///
    /// @param key Key/input from the user
    /// @return a unique pointer to another or same instance of Abstract State.
    virtual std::unique_ptr<AbstractState> handleInput(int key) = 0;

    /// @brief Default implementation for moving the cursor up.
    ///
    /// Override in in subclasses if cursor needs to move horizontally as well
    /// or constrained to a subspace of the window,etc.
    virtual void moveUp() {
      m_cursorPosition = std::max(0, m_cursorPosition - 1);
    };

    /// @brief Default implementation for moving the cursor down.
    ///
    /// Override in in subclasses if cursor needs to move horizontally as well
    /// or constrained to a subspace of the window,etc.
    virtual void moveDown() {
      m_cursorPosition = std::min(getmaxy(m_window), m_cursorPosition + 1);
    }

    /// @brief Name of state for purpose of logging/debugging.
    virtual std::string name() const = 0;

  protected:
    explicit AbstractState(
        WINDOW *window,
        std::shared_ptr<const Config::Config> config,
        IAppController &controller
    )
        : m_window(window), m_config(config), m_controller(controller),
          m_cursorPosition(0) {};

    WINDOW *m_window;
    std::shared_ptr<const Config::Config> m_config;
    IAppController &m_controller;
    int m_cursorPosition;

    template <typename T>
      requires std::derived_from<T, AbstractState>
    std::unique_ptr<T> makeState() {
      return std::make_unique<T>(m_window, m_config, m_controller);
    }
};

} // namespace QuickNotes::App::State
