#pragma once

#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/DeleteConfirmWidget.hpp"
#include <memory>
#include <vector>

namespace QuickNotes::App::State {

/// @brief Confirmation dialog state for deleting a note.
///
/// Presents a DeleteConfirmWidget asking the user to confirm or cancel.
/// On confirmation it calls INotesRepository::remove() and pops itself from
/// the stack; on cancel it pops without making any changes.
///
/// The note to be deleted and its index in the caller's list are passed in at
/// construction time.
///
/// @see NoteAwareState
/// @see UI::DeleteConfirmWidget
class DeleteNoteState : public NoteAwareState {
    using ConfigPtr = std::shared_ptr<const Config::Config>;

  public:
    /// @brief Constructs a DeleteNoteState.
    /// @param window Pointer to the ncurses WINDOW.
    /// @param config Shared pointer to the application configuration.
    /// @param controller Reference to the application controller.
    /// @param repository Reference to the notes repository.
    /// @param notes Reference to the caller's note list, updated on deletion.
    /// @param selectedIndex Index of the note in @p notes to be deleted.
    DeleteNoteState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::INotesRepository &repository,
        std::vector<Model::Note> &notes,
        int selectedIndex
    );

    /// @brief Route key input to confirm or cancel the deletion.
    /// @param key The raw ncurses key code.
    /// @return nullptr on cancel/confirm (both pop via controller).
    std::unique_ptr<AbstractState> handleInput(int key) override;

    /// @brief Render the confirmation dialog.
    void render() override;

    /// @brief Create and display the confirmation dialog widget.
    void onEnter() override;

    /// @brief Tear down the confirmation dialog widget.
    void onExit() override;

    std::string name() const override { return "DeleteNoteState"; }

  private:
    enum class DeleteConfirmAction { CONFIRM, CANCEL, NONE };
    std::unique_ptr<UI::DeleteConfirmWidget> m_widget;
    std::vector<Model::Note> &m_notes;
    Model::Note m_note;
    int m_selectedIndex;
    using Binding = std::pair<Config::Action, DeleteConfirmAction>;
    static const std::vector<Binding> m_keyMap;

    void handleConfirm();
};

} // namespace QuickNotes::App::State
