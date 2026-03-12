#pragma once

#include "AbstractState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "db/NoteRepository.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/NoteViewWidget.hpp"
#include <memory>

namespace QuickNotes::App::State {
class ViewingState : public NoteAwareState {
    using ConfigPtr = std::shared_ptr<Config::Config>;

  public:
    explicit ViewingState(
        WINDOW *window,
        ConfigPtr config,
        IAppController &controller,
        DB::NotesRepository &repository,
        Model::Note note
    ) noexcept;

    std::unique_ptr<AbstractState> handleInput(int key) override;

    void render() override;

    void onEnter() override;

    void onExit() override;

    void moveUp() override;

    void moveDown() override;

  private:
    enum class ViewActions {
      SCROLL_UP,
      SCROLL_DOWN,
      EDIT,
      BACK,
      NONE,
    };
    Model::Note m_note;
    std::unique_ptr<UI::NoteViewWidget> m_widget = nullptr;
    WINDOW *m_pad = nullptr;
    int m_scrollOffset;
    static constexpr int PAD_HEIGHT = 5000;
};
} // namespace QuickNotes::App::State
