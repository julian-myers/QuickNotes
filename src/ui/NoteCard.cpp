#include "NoteCard.hpp"
#include "markdown/Colors.hpp"
#include "models/Notes.hpp"
#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"
#include <string_view>

namespace QuickNotes::UI {

NoteCard::NoteCard(WINDOW *parent, Rect rect, Model::Note note)
    : Widget(parent),
      m_subWin(parent, rect),
      m_note(std::move(note)) {}

static void drawTagPill(WINDOW *win, int y, int &x, std::string_view tag) {
  wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_TAG));
  mvwaddstr(win, y, x, " ");
  mvwaddnstr(win, y, x + 1, tag.data(), tag.size());
  mvwaddstr(win, y, x + 1 + static_cast<int>(tag.size()), " ");
  wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_TAG));
  x += static_cast<int>(tag.size()) + 3;
}

void NoteCard::draw(bool selected, bool inSearch, bool inVisual) {
  WINDOW *win = m_subWin.get();
  int w = getmaxx(win) - 2;

  wclear(win);

  if (selected) {
    wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_CARD_SELECTED));
  } else if (inVisual) {
    wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_CARD_VISUAL));
  } else if (inSearch) {
    wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_HEADING_1));
  }

  wattron(win, A_BOLD | A_ITALIC);
  mvwprintw(win, 1, 1, "%-*.*s", w - 1, w - 1, m_note.title.c_str());
  wattroff(win, A_BOLD | A_ITALIC);

  if (selected) {
    wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_CARD_SELECTED));
  } else if (inVisual) {
    wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_CARD_VISUAL));
  } else if (inSearch) {
    wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_HEADING_1));
  }

  if (m_note.pinned) {
    wattron(win, A_BOLD);
    mvwaddstr(win, 1, getmaxx(win) - 4, "[*]");
    wattroff(win, A_BOLD);
  }

  mvwprintw(
      win, 2, 1, "Created: %-*s", w - 10, shortDate(m_note.createdAt).c_str()
  );
  mvwprintw(
      win, 3, 1, "Edited:  %-*s", w - 10, shortDate(m_note.updatedAt).c_str()
  );

  int x = 1;
  for (const auto &tag : m_note.tags) {
    if (x + static_cast<int>(tag.size()) + 3 > w) break;
    drawTagPill(win, 4, x, tag);
    ++x;
  }

  wnoutrefresh(win);
}

void NoteCard::draw() { draw(false, false); }

std::string NoteCard::shortDate(const std::string &date) {
  return date.size() >= 10 ? date.substr(0, 10) : date;
}

} // namespace QuickNotes::UI
