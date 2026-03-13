#pragma once

#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "db/NoteRepository.hpp"
#include "ncurses.h"
#include "ui/AddNoteWidget.hpp"
#include <memory>

namespace QuickNotes::App::State {

/// @brief Dialog box for adding new note.
class NewNoteState : public NoteAwareState {

  public:
    /// @brief Constructs a NewNoteState
    ///
    /// @param window pointer to ncurses WINDOW.
    /// @param config std::shared_ptr<Config::Config> to global config.
    /// @param controller Reference to instance of IAppController.
    /// @param notes Reference to instance of Model::Note.
    explicit NewNoteState(
        WINDOW *window,
        std::shared_ptr<const Config::Config> config,
        IAppController &controller,
        DB::NotesRepository &repository,
        std::vector<Model::Note> &notes
    ) noexcept;

    void onEnter() override;
    void onExit() override;
    void render() override;
    std::unique_ptr<AbstractState> handleInput(int key) override;

  private:
    enum class NewNoteAction { CONFIRM, CANCEL, NONE };
    std::unique_ptr<UI::AddNoteWidget> m_addNoteWidget;
    void createDialogBox();
    WINDOW *m_addNoteWindow;
    std::vector<Model::Note> &m_notes;
    std::string m_title = "";
    static const std::vector<std::pair<Config::Action, NewNoteAction>> m_keyMap;

    void handleConfirm();
};

} // namespace QuickNotes::App::State
