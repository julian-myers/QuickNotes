#include "ui/AddNoteWidget.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
namespace QuickNotes::UI {

AddNoteWidget::AddNoteWidget(WINDOW *window) : Widget(window) {
  int windowHeight = getmaxy(m_window);
  int windowWidth = getmaxx(m_window);
  int subWinYPos = (windowHeight - DIALOG_HEIGHT) / 2;
  int subWinXPos = (windowWidth - DIALOG_WIDTH) / 2;
  m_dialog =
      derwin(m_window, DIALOG_HEIGHT, DIALOG_WIDTH, subWinYPos, subWinXPos);
}

AddNoteWidget::~AddNoteWidget() {
  if (m_dialog) {
    delwin(m_dialog);
  }
}

void AddNoteWidget::draw() {
  wclear(m_dialog);
  box(m_dialog, 0, 0);
  mvwprintw(m_dialog, 1, MARGIN, LABEL);
  mvwhline(m_dialog, 2, 1, ACS_HLINE, DIALOG_WIDTH - 2);
  mvwprintw(m_dialog, 3, MARGIN, PROMPT);
  wrefresh(m_dialog);
}

std::string AddNoteWidget::getInput() {
  draw();
  echo();
  curs_set(1);
  char buf[128] = {};
  mvwgetnstr(m_dialog, 3, MARGIN + 7, buf, sizeof(buf) - 1);
  noecho();
  curs_set(0);
  return std::string(buf);
}

} // namespace QuickNotes::UI
