#pragma once

#include "AbstractState.hpp"
#include "app/Controller.hpp"
#include "app/state/NoteAwareState.hpp"
#include "config/Config.hpp"
#include "db/NoteRepository.hpp"
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
        DB::NotesRepository &repository
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
    static constexpr const char *SAMPLE = R"(
# Heading 1

Some **bold** and *italic* text, and `inline code`.

## Heading 2

> A block quote with some content.

- First item
- Second item
- Third item

1. Ordered one
2. Ordered two

---

| Name  | Age |
| :---- | --: |
| Alice | 30  |
| Bob   | 25  |
```cpp
int main() { return 0; }
` ``
)";

    Model::Note SAMPLE_NOTE = {
        0,
        std::string("sample"),
        SAMPLE,
        std::string("now"),
        std::string("now"),
    };

    void moveUp() override;

    void moveDown() override;

    /// @brief Returns the note from the list for which the cursor is on.
    Model::Note currentNote() const;

    /// @brief Open a note and transition to ViewingState.
    std::unique_ptr<AbstractState> select();

    /// @brief Transition to SearchListState.
    std::unique_ptr<AbstractState> search();

    void createWindow();

    void loadNotes();
};
} // namespace QuickNotes::App::State
