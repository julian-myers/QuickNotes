#pragma once

#include "AbstractState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
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

    /// @brief Translate a raw key into a MenuAction and act on it.
    ///
    /// Navigation keys update the selected index; the SELECT key triggers the
    /// highlighted option; letter shortcuts jump directly to Notes, Settings,
    /// or Quit.
    ///
    /// @param key The raw ncurses key code.
    /// @return A unique pointer to the next state, or nullptr to stay in this state.
    std::unique_ptr<AbstractState> handleInput(int key) override;

    /// @brief Render the main menu and recent-notes list.
    void render() override;

    /// @brief Load the most-recent notes and initialize the menu widget.
    void onEnter() override;

    /// @brief Clear the screen in preparation for the next state.
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
    std::vector<Model::Note> m_recentNotes;
    static constexpr int NUM_OPTIONS = 5;
    static const std::vector<std::pair<Config::Action, MenuAction>> m_keyMap;

    /// @brief Move the selection cursor down, clamped to the number of options.
    void moveDown() override;

    /// @brief Move the selection cursor up, clamped to zero.
    void moveUp() override;
};
} // namespace QuickNotes::App::State
