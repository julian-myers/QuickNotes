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

    /// @brief Renders the card with optional selection, search, and visual highlights.
    ///
    /// - @p selected: card is the cursor; drawn with PAIR_CARD_SELECTED.
    /// - @p inSearch: card is a search result; title drawn in PAIR_HEADING_1.
    /// - @p inVisual: card is in the visual selection range; title drawn with
    ///   PAIR_CARD_VISUAL (orange background).
    ///
    /// @param selected  Whether to draw the card with the selection color pair.
    /// @param inSearch  Whether the list is currently in search/navigate mode.
    /// @param inVisual  Whether the card is within the visual selection range.
    void draw(bool selected, bool inSearch = false, bool inVisual = false);

    /// @brief Renders the card in its default (unselected) state.
    void draw() override;

    /// @brief Fixed height of every NoteCard in terminal rows.
    static constexpr int HEIGHT = 5;

  private:
    SubWindow m_subWin;
    Model::Note m_note;

    /// @brief Formats an ISO-8601 timestamp as a short human-readable date.
    /// @param date The full datetime string from the database.
    /// @return A shortened date string for display on the card.
    static std::string shortDate(const std::string &date);
};

static void drawTagPill(WINDOW *win, int y, int &x, std::string_view tag);

} // namespace QuickNotes::UI
