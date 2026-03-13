#pragma once

#include "AbstractState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/NoteListWidget.hpp"
#include "ui/PreviewWidget.hpp"
#include <memory>
#include <ncurses.h>

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
    enum class ListActions {
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

    WINDOW *m_listWindow;
    WINDOW *m_previewWindow;
    int m_selectedIndex;
    std::unique_ptr<UI::NoteListWidget> m_list;
    std::unique_ptr<UI::PreviewWidget> m_preview;
    std::vector<Model::Note> m_notes;
    using Binding = std::pair<Config::Action, ListActions>;
    static const std::vector<Binding> m_keyMap;

    void moveUp() override;

    void moveDown() override;

    void createWindow();

    void loadNotes();
};
} // namespace QuickNotes::App::State
