#pragma once

#include "app/Controller.hpp"
#include "app/state/AbstractState.hpp"
#include "app/state/NoteAwareState.hpp"
#include "db/INoteRepository.hpp"
#include "ui/NoteContainer.hpp"
#include <string>

namespace QuickNotes::App::State {

class SearchingState : public NoteAwareState {
  public:
    explicit SearchingState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::INotesRepository &repository
    ) noexcept;

    void onEnter() override;

    void onExit() override;

    void render() override;

    std::string name() const override { return std::string("SearchingState"); }

    std::unique_ptr<AbstractState> handleInput(int key) override;

  private:
    enum class Actions { CONFIRM, BACKSPACE, APPEND, CANCEL, NONE };
    std::string m_inputBuffer;
    std::vector<Model::Note> m_filtered;
    std::unique_ptr<UI::NoteContainer> m_view;
};

} // namespace QuickNotes::App::State
