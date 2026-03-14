#include "NoteCard.hpp"
#include "markdown/Colors.hpp"
#include "models/Notes.hpp"
#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"

namespace QuickNotes::UI {

NoteCard::NoteCard(WINDOW *parent, Rect rect, Model::Note note)
    : Widget(parent), m_subWin(parent, rect), m_note(std::move(note)) {}

void NoteCard::draw(bool selected) {
  WINDOW *win = m_subWin.get();
  int w = getmaxx(win) - 2;

  wclear(win);

  if (selected) {
    wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_CARD_SELECTED));
  }

  wattron(win, A_BOLD | A_ITALIC);
  mvwprintw(win, 1, 1, "%-*.*s", w - 1, w - 1, m_note.title.c_str());
  wattroff(win, A_BOLD | A_ITALIC);

  mvwprintw(
      win, 2, 1, "Created: %-*s", w - 10, shortDate(m_note.createdAt).c_str()
  );

  mvwprintw(
      win, 3, 1, "Edited:  %-*s", w - 10, shortDate(m_note.updatedAt).c_str()
  );

  mvwprintw(win, 4, 1, "%-*s", w - 10, "");

  if (selected) {
    wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_CARD_SELECTED));
  }

  wnoutrefresh(win);
}

void NoteCard::draw() { draw(false); }

std::string NoteCard::shortDate(const std::string &date) {
  return date.size() >= 10 ? date.substr(0, 10) : date;
}

} // namespace QuickNotes::UI
