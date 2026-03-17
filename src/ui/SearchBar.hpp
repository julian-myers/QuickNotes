#pragma once

#include "ui/Widget.hpp"
#include <string>
#include <string_view>

namespace QuickNotes::UI {

/// @brief Single-row search input bar shown at the bottom of the note list.
///
/// Displays a label (e.g. "/") followed by the current query text.
/// Purely presentational — NoteListState owns the query string and passes
/// it in via setInputBuffer() before each draw() call.
///
/// @see NoteContainer
/// @see Widget
class SearchBar : public Widget {

  public:
    /// @brief Constructs a SearchBar.
    /// @param parent Pointer to the ncurses WINDOW to render into.
    explicit SearchBar(WINDOW *parent);

    /// @brief Renders the label and input buffer to m_window.
    void draw() override;

    /// @brief Sets the query text displayed after the label.
    /// @param buffer The current search query.
    void setInputBuffer(std::string_view buffer);

    /// @brief Sets the label shown before the query (e.g. "/").
    /// @param label The prefix label.
    void setLabel(std::string_view label);

  private:
    std::string m_inputBuffer;
    std::string m_label;
};

} // namespace QuickNotes::UI
