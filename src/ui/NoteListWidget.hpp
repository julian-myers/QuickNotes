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

    /// @brief draw content to window.
    /// @param notes list of notes to display.
    void draw(const Notes &notes, int selectedIndex);

    /// @brief draw content to window.
    /// @param notes list of notes to display.
    void draw(const Notes &notes);

    /// @brief Get the selected index.
    /// @return the index of the note list that gets selected.
    int selectedIndex();

  private:
    std::vector<NoteCard> m_cards;
    std::vector<int> m_cachedIds;
    static constexpr int HEADER_HEIGHT = 3;

    bool notesChanged(const Notes &notes) const;

    void rebuildCards(const Notes &notes);
};

} // namespace QuickNotes::UI
