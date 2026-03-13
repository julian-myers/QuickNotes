#include "ui/StatusBar.hpp"
#include "markdown/Colors.hpp"
#include "ui/Widget.hpp"

namespace QuickNotes::UI {

StatusBar::StatusBar(WINDOW *window) : Widget(window) {}

void StatusBar::setLabel(std::string_view label) { m_label = label; }

void StatusBar::setInputBuffer(std::string_view buffer) {
  m_inputBuffer = buffer;
}

void StatusBar::draw() {
  int row = getmaxy(m_window) - 1;
  int width = getmaxx(m_window);

  wattron(m_window, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);
  mvwprintw(m_window, row, 0, "%-*s", width, ""); // clear the row
  mvwprintw(m_window, row, 0, "%s", m_label.c_str());
  wattroff(m_window, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);

  if (!m_inputBuffer.empty()) {
    mvwprintw(
        m_window,
        row,
        static_cast<int>(m_label.size()) + 1,
        "%s",
        m_inputBuffer.c_str()
    );
  }

  wnoutrefresh(m_window);
}

} // namespace QuickNotes::UI
