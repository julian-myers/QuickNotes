#include "DeleteConfirmWidget.hpp"
#include "markdown/Colors.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <cstring>

namespace QuickNotes::UI {

DeleteConfirmWidget::DeleteConfirmWidget(WINDOW *window) : Widget(window) {
  int windowHeight = getmaxy(m_window);
  int windowWidth = getmaxx(m_window);
  int subWinYPos = (windowHeight - DIALOG_HEIGHT) / 2;
  int subWinXPos = (windowWidth - DIALOG_WIDTH) / 2;
  m_dialog =
      derwin(m_window, DIALOG_HEIGHT, DIALOG_WIDTH, subWinYPos, subWinXPos);
}

DeleteConfirmWidget::~DeleteConfirmWidget() {
  if (m_dialog) {
    delwin(m_dialog);
  }
}

void DeleteConfirmWidget::draw() { draw(""); }

void DeleteConfirmWidget::draw(const std::string &title) {
  wclear(m_dialog);
  box(m_dialog, 0, 0);
  wattron(m_dialog, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);
  mvwprintw(m_dialog, 1, MARGIN, "%s", LABEL);
  wattroff(m_dialog, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);
  mvwhline(m_dialog, 2, 1, ACS_HLINE, DIALOG_WIDTH - 2);
  std::string truncated = title;
  int maxTitleWidth = DIALOG_WIDTH - (MARGIN * 2) - 2;
  if (static_cast<int>(truncated.size()) > maxTitleWidth) {
    truncated = truncated.substr(0, maxTitleWidth - 3) + "...";
  }
  mvwprintw(m_dialog, 3, MARGIN, "%s \"%s\"", PROMPT, truncated.c_str());
  wattron(m_dialog, COLOR_PAIR(Markdown::Colors::PAIR_ITALIC) | A_ITALIC);
  mvwprintw(m_dialog, 5, (DIALOG_WIDTH - static_cast<int>(strlen(OPTIONS))) / 2,
            "%s", OPTIONS);
  wattroff(m_dialog, COLOR_PAIR(Markdown::Colors::PAIR_ITALIC) | A_ITALIC);
  wrefresh(m_dialog);
}

} // namespace QuickNotes::UI
