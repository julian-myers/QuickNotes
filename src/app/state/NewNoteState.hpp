#pragma once

#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "ncurses.h"
#include "ui/AddNoteWidget.hpp"
#include <functional>
#include <memory>

namespace QuickNotes::App::State {

/// @brief Dialog box for adding new note.
///
/// Owns the input buffer and drives all keyboard handling for the
/// add-note dialog. AddNoteWidget is purely presentational — it renders
/// whatever buffer this state provides.
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
        DB::INotesRepository &repository,
        std::vector<Model::Note> &notes
    ) noexcept;

    void onEnter() override;
    void onExit() override;
    void render() override;
    std::unique_ptr<AbstractState> handleInput(int key) override;
    std::string name() const override { return "NewNoteState"; }

  private:
    enum class NewNoteAction { CONFIRM, CANCEL, APPEND, BACKSPACE, NONE };

    std::unique_ptr<UI::AddNoteWidget> m_widget;
    std::vector<Model::Note> &m_notes;
    std::string m_inputBuffer;

    void createDialogBox();

    void handleConfirm();

    NewNoteAction toAction(int key) const;
};

} // namespace QuickNotes::App::State
