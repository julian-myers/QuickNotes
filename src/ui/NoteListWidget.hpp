#pragma once

#include "Widget.hpp"
#include "models/Notes.hpp"
#include "ui/NoteCard.hpp"
#include <vector>

namespace QuickNotes::UI {

/// @brief Scrollable list of NoteCard widgets for the note browser.
///
/// Rebuilds its internal card vector only when the set of note IDs changes,
/// avoiding unnecessary SubWindow reallocations on every frame. The currently
/// selected card is highlighted by passing the selected index to the
/// NoteCard::draw(bool) overload.
///
/// @see NoteCard
/// @see NoteContainer
/// @see Widget
class NoteListWidget : public Widget {
    using Notes = std::vector<Model::Note>;

  public:
    /// @brief Constructs a NoteListWidget.
    /// @param window pointer to ncurses window.
    explicit NoteListWidget(WINDOW *window);

    void draw() override;

    /// @brief Render the list in normal mode.
    /// @param notes         Notes to display.
    /// @param selectedIndex Index of the highlighted note.
    void draw(const Notes &notes, int selectedIndex);

    /// @brief Render the list in search/navigate mode.
    ///
    /// Unselected titles are drawn with the search highlight color.
    /// @param notes         Filtered notes to display.
    /// @param selectedIndex Index of the highlighted note.
    /// @param inSearch      Pass true to apply search title colors.
    void draw(const Notes &notes, int selectedIndex, bool inSearch);

    /// @brief Render without selection (all cards unselected).
    /// @param notes list of notes to display.
    void draw(const Notes &notes);

    /// @brief Get the selected index.
    /// @return the index of the note list that gets selected.
    int selectedIndex();

  private:
    std::vector<NoteCard> m_cards;
    std::vector<std::pair<int, std::string>> m_cachedKeys;
    static constexpr int HEADER_HEIGHT = 3;

    bool notesChanged(const Notes &notes) const;

    void rebuildCards(const Notes &notes);
};

} // namespace QuickNotes::UI
