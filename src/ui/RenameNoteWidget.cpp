#include "ui/RenameNoteWidget.hpp"
#include "ui/DialogBox.hpp"
#include <ncurses.h>

namespace QuickNotes::UI {

RenameNoteWidget::RenameNoteWidget(WINDOW *parent)
    : DialogBox(parent, DIALOG_HEIGHT, DIALOG_WIDTH) {}

void RenameNoteWidget::setInputBuffer(std::string_view buffer) {
  m_inputBuffer = buffer;
}

void RenameNoteWidget::setError(std::string_view message) {
  m_errorMessage = message;
}

void RenameNoteWidget::draw() {
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
