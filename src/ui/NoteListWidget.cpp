#include "NoteListWidget.hpp"
#include "markdown/Colors.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/NoteCard.hpp"
#include "ui/Widget.hpp"
#include <vector>

namespace QuickNotes::UI {

NoteListWidget::NoteListWidget(WINDOW *window) : Widget(window) {}

void NoteListWidget::draw(
    const std::vector<Model::Note> &notes, int selectedIndex
) {
  std::vector<int> ids;
  ids.reserve(notes.size());
  for (const auto &note : notes) {
    ids.push_back(note.id);
  }
  if (ids != m_cachedIds) {
    rebuildCards(notes);
    m_cachedIds = std::move(ids);
  }

  clear();
  int margin = 1;
  drawBorder();
  attrOn(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);
  print(0, 2, " Notes:");
  attrOff(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);

  for (int i = 0; i < static_cast<int>(m_cards.size()); i++) {
    int y = HEADER_HEIGHT + i * NoteCard::HEIGHT;
    if (y + NoteCard::HEIGHT > height() - 1) break;
    m_cards[i].draw(i == selectedIndex);
  }

  wnoutrefresh(m_window);
}

void NoteListWidget::draw() {}

void NoteListWidget::rebuildCards(const Notes &notes) {
  m_cards.clear();
  for (int i = 0; i < static_cast<int>(notes.size()); ++i) {
    int row = HEADER_HEIGHT + i * NoteCard::HEIGHT;
    if (row + NoteCard::HEIGHT - 1 >= height()) break;
    Rect rect{row, 2, NoteCard::HEIGHT - 1, width() - 4};
    m_cards.emplace_back(m_window, rect, notes[i]);
  }
}

} // namespace QuickNotes::UI
