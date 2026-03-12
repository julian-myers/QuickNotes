#pragma once

#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "db/NoteRepository.hpp"
#include "models/Notes.hpp"
#include "ui/DeleteConfirmWidget.hpp"
#include <memory>
#include <vector>

namespace QuickNotes::App::State {

class DeleteNoteState : public NoteAwareState {
    using ConfigPtr = std::shared_ptr<Config::Config>;

  public:
    DeleteNoteState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::NotesRepository &repository,
        std::vector<Model::Note> &notes,
        int selectedIndex
    );

    std::unique_ptr<AbstractState> handleInput(int key) override;
    void render() override;
    void onEnter() override;
    void onExit() override;

  private:
    enum class DeleteConfirmAction { CONFIRM, CANCEL, NONE };
    std::unique_ptr<UI::DeleteConfirmWidget> m_widget;
    std::vector<Model::Note> &m_notes;
    Model::Note m_note;
    int m_selectedIndex;

    void handleConfirm();
};

} // namespace QuickNotes::App::State
