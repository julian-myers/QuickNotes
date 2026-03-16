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
    enum class Mode { NORMAL, SEARCH };

    enum class NormalAction {
      MOVE_UP,
      MOVE_DOWN,
      VIEW_NOTE,
      SEARCH,
      SELECT,
      NEW_NOTE,
      DELETE_NOTE,
      QUIT,
      NONE,
    };

    int m_selectedIndex;
    Mode m_mode = Mode::NORMAL;
    std::string m_query;
    std::string m_inputBuffer;
    std::vector<Model::Note> m_notes;
    std::vector<Model::Note> m_results;
    std::unique_ptr<UI::NoteContainer> m_view;

    using Binding = std::pair<Config::Action, NormalAction>;
    static const std::vector<Binding> m_keyMap;

    std::unique_ptr<AbstractState> handleNormal(int key);
    std::unique_ptr<AbstractState> handleSearch(int key);

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
