#pragma once

#include "Widget.hpp"
#include "models/Notes.hpp"
#include <vector>

namespace QuickNotes::UI {

class NoteListWidget : public Widget {
    using Notes = std::vector<Model::Note>;

  public:
    /// @brief Constructs a NoteListWidget.
    /// @param window pointer to ncurses window.
    explicit NoteListWidget(WINDOW *window);

    void draw() override;

    /// @brief draw content to window.
    /// @param notes list of notes to display.
    void draw(Notes notes, int selectedIndex);

    /// @brief Get the selected index.
    /// @return the index of the note list that gets selected.
    int selectedIndex();

    /// @brief Get the beginning row of the window.
    /// @return row number of window.
    int startingRow();

    /// @brief Get the beginning column of the window.
    /// @return column number of window.
    int startingColumn();
};

} // namespace QuickNotes::UI
