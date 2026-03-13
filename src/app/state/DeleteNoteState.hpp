#pragma once

#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/DeleteConfirmWidget.hpp"
#include <memory>
#include <vector>

namespace QuickNotes::App::State {

class DeleteNoteState : public NoteAwareState {
    using ConfigPtr = std::shared_ptr<const Config::Config>;

  public:
    DeleteNoteState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::INotesRepository &repository,
        std::vector<Model::Note> &notes,
        int selectedIndex
    );

    std::unique_ptr<AbstractState> handleInput(int key) override;
    void render() override;
    void onEnter() override;
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
