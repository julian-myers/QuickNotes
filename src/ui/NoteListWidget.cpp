#include "NoteListWidget.hpp"
#include "markdown/Colors.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <vector>

namespace QuickNotes::UI {

NoteListWidget::NoteListWidget(WINDOW *window)
    : Widget(window), m_windowHeight(getmaxy(m_window)),
      m_windowWidth(getmaxx(m_window)), m_startRow(0), m_startColumn(0),
      m_selectedIndex(0) {}

void NoteListWidget::draw(std::vector<Model::Note> notes) {
  wclear(m_window);

  static constexpr int MARGIN = 2;
  static constexpr int HEADER_ROW = 1;
  static constexpr int LIST_START_ROW = 3;

  static const char *header = "Notes:";
  wattron(m_window, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);
  mvwprintw(m_window, m_startRow + 4, m_startColumn + 4, "%s", header);
  wattroff(m_window, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD);

  mvwhline(m_window, HEADER_ROW + 1, 0, ACS_HLINE, m_windowWidth);
  for (int i = 0; i < static_cast<int>(notes.size()); i++) {
    int row = LIST_START_ROW + i;
    if (row >= m_windowHeight - 1) break;
    if (i == m_selectedIndex) {
      wattron(m_window, A_REVERSE);
    }

    mvwprintw(
        m_window,
        row,
        MARGIN,
        "%-*s",
        m_windowWidth - (MARGIN * 2),
        notes[i].title.c_str()
    );
    if (i == m_selectedIndex) {
      wattroff(m_window, A_REVERSE);
    }
  }
  mvwvline(m_window, 0, m_windowWidth - 1, ACS_VLINE, m_windowWidth);
  wnoutrefresh(m_window);
}

void NoteListWidget::draw() {
  static const char *header = "notes";
  mvwprintw(m_window, m_startRow, m_startColumn, "%s", header);
  mvwvline(m_window, 0, m_windowWidth - 1, ACS_VLINE, m_windowHeight);
}

void NoteListWidget::setSelectedIndex(int index) { m_selectedIndex = index; }

int NoteListWidget::selectedIndex() { return m_selectedIndex; }

int NoteListWidget::windowWidth() { return m_windowWidth; }

int NoteListWidget::windowHeight() { return m_windowHeight; }

int NoteListWidget::startingRow() { return m_startRow; }

int NoteListWidget::startingColumn() { return m_startColumn; }

} // namespace QuickNotes::UI
