#include "ui/AddNoteWidget.hpp"
#include "ncurses.h"
#include "ui/DialogBox.hpp"
#include "ui/Widget.hpp"
#include <ncurses.h>

namespace QuickNotes::UI {

AddNoteWidget::AddNoteWidget(WINDOW *parent)
    : DialogBox(parent, DIALOG_HEIGHT, DIALOG_WIDTH) {}

void AddNoteWidget::setInputBuffer(std::string_view buffer) {
  m_inputBuffer = buffer;
}

void AddNoteWidget::setError(std::string_view message) {
  m_errorMessage = message;
}

void AddNoteWidget::draw() {
  drawChrome(LABEL);
  WINDOW *win = m_dialog.get();
  roundedBox(win);
  mvwprintw(
      win,
      3,
      MARGIN,
      "%.*s%s",
      static_cast<int>(PROMPT.size()),
      PROMPT.data(),
      m_inputBuffer.c_str()
  );
  if (!m_errorMessage.empty()) {
    wattron(win, A_BOLD);
    mvwprintw(
        win, 4, MARGIN, "%-*.*s",
        DIALOG_WIDTH - (MARGIN * 2) - 2,
        DIALOG_WIDTH - (MARGIN * 2) - 2,
        m_errorMessage.c_str()
    );
    wattroff(win, A_BOLD);
  }
  wrefresh(win);
}
} // namespace QuickNotes::UI
