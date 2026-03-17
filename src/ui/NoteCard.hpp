#pragma once

#include "models/Notes.hpp"
#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"

namespace QuickNotes::UI {

/// @brief Compact card widget that renders a single note's summary.
///
/// Occupies a fixed HEIGHT of 4 rows and displays the note title and a
/// shortened date. The selected variant highlights the card with a different
/// color pair.
///
/// Each card owns a SubWindow derived from the parent window at the Rect
/// provided at construction.
///
/// @see NoteListWidget
/// @see Widget
class NoteCard : public Widget {
  public:
    /// @brief Constructs a NoteCard.
    /// @param parent Pointer to the parent ncurses WINDOW.
    /// @param rect   Position and dimensions within the parent.
    /// @param note   The note whose summary this card displays.
    explicit NoteCard(WINDOW *parent, Rect rect, Model::Note note);

    /// @brief Renders the card, optionally highlighted as selected.
    /// @param selected Whether to draw the card with the selection color pair.
    void draw(bool selected);

    /// @brief Renders the card in its default (unselected) state.
    void draw() override;

    /// @brief Fixed height of every NoteCard in terminal rows.
    static constexpr int HEIGHT = 4;

  private:
    SubWindow m_subWin;
    Model::Note m_note;

    /// @brief Formats an ISO-8601 timestamp as a short human-readable date.
    /// @param date The full datetime string from the database.
    /// @return A shortened date string for display on the card.
    static std::string shortDate(const std::string &date);
};

} // namespace QuickNotes::UI
