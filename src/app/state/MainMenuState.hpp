#pragma once

#include "AbstractState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "ui/MainMenu.hpp"
#include <memory>
#include <ncurses.h>

namespace QuickNotes::App::State {

/// @brief Renders the main menu screens and handles menu navigation.
///
/// Owns a MainMenu widget and is responsible for transitioning to other states
/// based on the user's selection. Part of the application state machine rooted
/// at App.
///
/// @see AbstractState
/// @see MainMenu
class MainMenuState : public NoteAwareState {

  public:
    /// @brief Constructs a MainMenuState.
    /// @param window pointer to the ncurses window this state renders in
    /// @param config shared pointer to the application configuration.
    explicit MainMenuState(
        WINDOW *window,
        std::shared_ptr<const Config::Config> config,
        IAppController &controller,
        DB::INotesRepository &repository
    ) noexcept;

    /// @brief Maps key/user input to semantic action to which informs widget
    /// which action to execute/respond to.
    /// @param key The key pressed/input.
    /// @return A unique pointer that points to the next state to transition to.
    std::unique_ptr<AbstractState> handleInput(int key) override;

    /// @brief Render the menu.
    void render() override;

    void onEnter() override;

    void onExit() override;

    std::string name() const override { return "MainMenuState"; }

  private:
    ///@brief Set of possible menu actions.
    enum class MenuAction {
      MOVE_UP,
      MOVE_DOWN,
      SELECT,
      NOTES_SELECTED,
      SETTINGS_SELECTED,
      QUIT_SELECTED,
      NONE,
    };
    enum class MenuOption { NOTES = 0, SETTINGS = 1, EXIT = 2 };
    UI::MainMenu m_menu;
    int m_selectedIndex = 0;
    static constexpr int NUM_OPTIONS = 5;
    static const std::vector<std::pair<Config::Action, MenuAction>> m_keyMap;

    /// @brief Move cursor down.
    void moveDown() override;
};
} // namespace QuickNotes::App::State
