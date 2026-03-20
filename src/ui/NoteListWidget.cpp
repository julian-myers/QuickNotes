#include "NoteListWidget.hpp"
#include "markdown/Colors.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/NoteCard.hpp"
#include "ui/Widget.hpp"
#include <algorithm>
#include <vector>

namespace QuickNotes::UI {

static std::string noteKey(const Model::Note &note) {
  std::string k = note.title + (note.pinned ? "|1" : "|0");
  for (const auto &t : note.tags) { k += '|'; k += t; }
  return k;
}

NoteListWidget::NoteListWidget(WINDOW *window) : Widget(window) {}

void NoteListWidget::draw(
    const std::vector<Model::Note> &notes, int selectedIndex
) {
  draw(notes, selectedIndex, false);
}

void NoteListWidget::draw(
    const std::vector<Model::Note> &notes, int selectedIndex, bool inSearch
) {
  std::vector<std::pair<int, std::string>> keys;
  keys.reserve(notes.size());
  for (const auto &note : notes) {
    keys.emplace_back(note.id, noteKey(note));
  }
  if (keys != m_cachedKeys) {
    rebuildCards(notes);
    m_cachedKeys = std::move(keys);
  }

  clear();
  if (m_visual) {
    drawBorder(COLOR_PAIR(Markdown::Colors::PAIR_VISUAL_ACCENT));
  } else {
    drawBorder();
  }
  attrOn(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);
  print(0, 2, inSearch ? " Results:" : " Notes:");
  attrOff(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);

  for (int i = 0; i < static_cast<int>(m_cards.size()); i++) {
    int y = HEADER_HEIGHT + i * NoteCard::HEIGHT;
    if (y + NoteCard::HEIGHT > height() - 1) break;
    m_cards[i].draw(i == selectedIndex, inSearch);
  }

  wnoutrefresh(m_window);
}

void NoteListWidget::draw(const std::vector<Model::Note> &notes) {
  draw(notes, -1, false);
}

void NoteListWidget::draw(
    const Notes &notes, int selectedIndex, int visualStart
) {
  std::vector<std::pair<int, std::string>> keys;
  keys.reserve(notes.size());
  for (const auto &note : notes) {
    keys.emplace_back(note.id, noteKey(note));
  }
  if (keys != m_cachedKeys) {
    rebuildCards(notes);
    m_cachedKeys = std::move(keys);
  }

  clear();
  if (m_visual) {
    drawBorder(COLOR_PAIR(Markdown::Colors::PAIR_VISUAL_ACCENT));
  } else {
    drawBorder();
  }
  attrOn(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);
  print(0, 2, " Notes:");
  attrOff(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);

  const int lo = std::min(selectedIndex, visualStart);
  const int hi = std::max(selectedIndex, visualStart);

  for (int i = 0; i < static_cast<int>(m_cards.size()); i++) {
    int y = HEADER_HEIGHT + i * NoteCard::HEIGHT;
    if (y + NoteCard::HEIGHT > height() - 1) break;
    const bool isCursor  = (i == selectedIndex);
    const bool inRange   = (i >= lo && i <= hi && !isCursor);
    m_cards[i].draw(isCursor, false, inRange);
  }

  wnoutrefresh(m_window);
}

void NoteListWidget::draw() {}

void NoteListWidget::setVisual(bool visual) { m_visual = visual; }

void NoteListWidget::rebuildCards(const Notes &notes) {
  m_cards.clear();
  for (int i = 0; i < static_cast<int>(notes.size()); ++i) {
    int row = HEADER_HEIGHT + i * NoteCard::HEIGHT;
    if (row + NoteCard::HEIGHT - 1 >= height()) break;
    Rect rect{row, 2, NoteCard::HEIGHT, width() - 4};
    m_cards.emplace_back(m_window, rect, notes[i]);
  }
}

} // namespace QuickNotes::UI
