#pragma once

#include "AbstractState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/NoteContainer.hpp"
#include <memory>
#include <ncurses.h>
#include <string_view>

namespace QuickNotes::App::State {

/// @brief Displays the full note list and handles browsing, searching, and
/// previewing.
///
/// Manages three sub-modes:
/// - NORMAL: navigate the note list with cursor movement, open or delete notes.
/// - SEARCH: filter notes in real time as the user types a query.
/// - PREVIEW: scroll through the rendered markdown preview of the selected note.
///
/// Owns a NoteContainer widget that composites the list panel, preview panel,
/// search bar, and status bar. Input is routed to one of handleNormal(),
/// handleSearch(), or handlePreview() depending on the current mode.
///
/// @see NoteAwareState
/// @see UI::NoteContainer
class NoteListState : public NoteAwareState {
    using ConfigPtr = std::shared_ptr<const Config::Config>;

  public:
    /// @brief Constructs a NoteListState
    /// @param window Instance of ncurses window.
    /// @param config
    explicit NoteListState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::INotesRepository &repository
    ) noexcept;

    /// @brief Handle user input
    /// @param key user pressed key.
    /// @return a unique pointer to an implementation of AbstractState.
    std::unique_ptr<AbstractState> handleInput(int key) override;

    /// @brief Render the notes list widgets.
    void render() override;

    /// @brief Initialize subwindows.
    void onEnter() override;

    /// @brief teardown subwindows.
    void onExit() override;

    std::string name() const override { return "NoteListState"; }

  private:
    enum class Mode { NORMAL, SEARCH, PREVIEW };

    enum class NormalAction {
      MOVE_UP,
      MOVE_DOWN,
      FOCUS_PREVIEW,
      SEARCH,
      EDIT_NOTE,
      NEW_NOTE,
      DELETE_NOTE,
      RENAME_NOTE,
      QUIT,
      NONE,
    };

    using Binding = std::pair<Config::Action, NormalAction>;
    int m_selectedIndex;
    Mode m_mode = Mode::NORMAL;
    std::string m_query;
    std::string m_inputBuffer;
    std::vector<Model::Note> m_notes;
    std::vector<Model::Note> m_results;
    std::unique_ptr<UI::NoteContainer> m_view;
    static const std::vector<Binding> m_keyMap;
    bool m_lastStateWasSearch = false;
    bool m_searchTyping = false;

    std::unique_ptr<AbstractState> handleNormal(int key);
    std::unique_ptr<AbstractState> handleSearch(int key);
    std::unique_ptr<AbstractState> handlePreview(int key);

    void enterSearchMode();
    void exitSearchMode();
    void updateSearch();

    std::vector<Model::Note> &activeNotes();
    std::string_view modeLabel() const;

    void moveUp() override;
    void moveDown() override;

    void loadNotes();
};
} // namespace QuickNotes::App::State
