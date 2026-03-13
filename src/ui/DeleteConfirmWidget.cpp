#include "DeleteConfirmWidget.hpp"
#include "markdown/Colors.hpp"
#include "ncurses.h"
#include "ui/DialogBox.hpp"
#include <string_view>

namespace QuickNotes::UI {

DeleteConfirmWidget::DeleteConfirmWidget(WINDOW *parent)
    : DialogBox(parent, DIALOG_HEIGHT, DIALOG_WIDTH) {}

void DeleteConfirmWidget::setNoteTitle(std::string_view title) {
  m_noteTitle = title;
}

void DeleteConfirmWidget::draw() {
  drawChrome(LABEL);
  WINDOW *win = m_dialog.get();
  int maxTitleWidth = DIALOG_WIDTH - (MARGIN * 2) - 2;
  std::string truncated = m_noteTitle;
  if (static_cast<int>(truncated.size()) > maxTitleWidth) {
    truncated = truncated.substr(0, maxTitleWidth - 3) + "...";
  }

  mvwprintw(
      win,
      3,
      MARGIN,
      "%.*s \"%s\"",
      static_cast<int>(PROMPT.size()),
      PROMPT.data(),
      truncated.c_str()
  );

  wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_ITALIC) | A_ITALIC);
  mvwprintw(
      win,
      5,
      (DIALOG_WIDTH - static_cast<int>(OPTIONS.size())) / 2,
      "%.*s",
      static_cast<int>(OPTIONS.size()),
      OPTIONS.data()
  );
  wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_ITALIC) | A_ITALIC);

  wrefresh(win);
}

} // namespace QuickNotes::UI
