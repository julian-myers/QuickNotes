#include "DialogBox.hpp"
#include "markdown/Colors.hpp"
#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"
#include <ncurses.h>
#include <string_view>

namespace QuickNotes::UI {

DialogBox::DialogBox(WINDOW *parent, int height, int width)
    : Widget(parent), m_dialog(parent, centeredRect(parent, height, width)),
      m_height(height), m_width(width) {}

void DialogBox::resize(WINDOW *window) {
  m_window = window;
  m_dialog = SubWindow(window, centeredRect(window, m_height, m_width));
}

void DialogBox::drawChrome(std::string_view title) {
  int margin = 2;
  WINDOW *win = m_dialog.get();
  wclear(win);
  roundedBox(win);
  wattron(win, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);
  mvwprintw(
      win, 1, margin, "%.*s", static_cast<int>(title.size()), title.data()
  );
  wattroff(win, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);
  mvwhline(win, 2, 1, ACS_HLINE, m_width - 2);
}

Rect DialogBox::centeredRect(WINDOW *parent, int height, int width) {
  int parentHeight = getmaxy(parent);
  int parentWidth = getmaxx(parent);
  return Rect{
      .yPos = (parentHeight - height) / 2,
      .xPos = (parentWidth - width) / 2,
      .height = height,
      .width = width,
  };
}

} // namespace QuickNotes::UI
