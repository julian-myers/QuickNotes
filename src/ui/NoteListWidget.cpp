#include "NoteListWidget.hpp"
#include "markdown/Colors.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <vector>

namespace QuickNotes::UI {

NoteListWidget::NoteListWidget(WINDOW *window) : Widget(window) {}

void NoteListWidget::draw(std::vector<Model::Note> notes, int selectedIndex) {
  clear();

  int margin = 1;

  drawBorder();
  attrOn(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);
  print(0, 2, " Notes:");
  attrOff(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);

  for (int i = 0; i < static_cast<int>(notes.size()); i++) {
    int row = 3 + i;
    if (row >= height() - 1) break;
    if (i == selectedIndex) attrOn(A_REVERSE);
    printClipped(row, 2, notes[i].title, width() - 4);
    if (i == selectedIndex) attrOff(A_REVERSE);
  }
  wnoutrefresh(m_window);
}

void NoteListWidget::draw() {}

} // namespace QuickNotes::UI
