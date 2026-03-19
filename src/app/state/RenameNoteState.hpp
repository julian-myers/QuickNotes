#pragma once

#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "db/INoteRepository.hpp"
#include "models/Notes.hpp"
#include "ui/RenameNoteWidget.hpp"
#include <memory>
#include <vector>

namespace QuickNotes::App::State {

/// @brief Dialog state for renaming an existing note.
///
/// Owns the input buffer and drives all keyboard handling for the rename
/// dialog. RenameNoteWidget is purely presentational — it renders whatever
/// buffer this state provides.
class RenameNoteState : public NoteAwareState {
  public:
    RenameNoteState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::INotesRepository &repository,
        std::vector<Model::Note> &notes,
        int selectedIndex
    );

    void onEnter() override;
    void onExit() override;
    void render() override;
    std::unique_ptr<AbstractState> handleInput(int key) override;
    std::string name() const override { return "RenameNoteState"; }

  private:
    enum class RenameAction { CONFIRM, CANCEL, APPEND, BACKSPACE, NONE };

    std::unique_ptr<UI::RenameNoteWidget> m_widget;
    std::vector<Model::Note> &m_notes;
    int m_selectedIndex;
    std::string m_inputBuffer;

    void handleConfirm();
    RenameAction toAction(int key) const;
};

} // namespace QuickNotes::App::State
