#include "MainMenu.hpp"
#include "markdown/Colors.hpp"
#include "ui/Widget.hpp"
#include <algorithm>
#include <cstring>
#include <ncurses.h>
#include <tuple>
#include <vector>

namespace QuickNotes::UI {

// --- Public Methods ---

// Constructor
MainMenu::MainMenu(WINDOW *window)
    : Widget(window), m_winHeight(getmaxy(m_window)),
      m_winWidth(getmaxx(m_window)),
      m_titleWidth(static_cast<int>(strlen(LINE_1 + 1))),
      m_startRow(std::max(0, (m_winHeight - TITLE_HEIGHT) / 2) - TITLE_HEIGHT),
      m_startCol(std::max(0, (m_winWidth - m_titleWidth) / 2)) {}

// draw the whole widget to screen.
void MainMenu::draw() {
  wclear(m_window);
  drawTitle();
  auto dims = drawOptions();
  int optionRow = std::get<0>(dims);
  int range = std::get<1>(dims);
  int margin = std::get<2>(dims);
  drawRecentNotes(optionRow, range, margin);
  wrefresh(m_window);
}

// --- Private Methods ---

// Draws the title.
void MainMenu::drawTitle() {
  wattron(m_window, COLOR_PAIR(Markdown::Colors::PAIR_HEADING_1));
  for (int i = 0; i < TITLE_HEIGHT; i++) {
    mvwprintw(m_window, m_startRow + i, m_startCol, "%s", TITLE[i] + 1);
  }
  wattroff(m_window, COLOR_PAIR(Markdown::Colors::PAIR_HEADING_1));
}

// Draws the hot key options under the title in a row.
std::tuple<int, int, int> MainMenu::drawOptions() {
  std::string notesOption = "󱔗 Notes[n]";
  std::string settingsOption = " Settings[s]";
  std::string quitOption = "󰿅 Quit[q]";
  std::vector<std::string> options = {notesOption, settingsOption, quitOption};
  int numOptions = options.size();
  int optionRow = m_startRow + TITLE_HEIGHT + 1;
  int margin = m_winWidth / 3;
  int range = m_winWidth - (margin * 2);
  int spacing = range / (numOptions - 1);
  wattron(m_window, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_ITALIC);
  for (int i = 0; i < numOptions; i++) {
    int col = margin + spacing * i - (static_cast<int>(options[i].size()) / 2);
    mvwprintw(m_window, optionRow, col, "%s", options[i].c_str());
  }
  wattroff(m_window, COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_ITALIC);
  return std::make_tuple(optionRow, range, margin);
}

// Draw the list of recently opened notes under the options.
void MainMenu::drawRecentNotes(int optionRow, int range, int margin) {
  int subHeight = static_cast<int>(m_recentNotes.size()) + 2;
  int subRow = optionRow + 2;
  int subCol = margin;
  std::string heading = "Recent Notes: ";
  wattron(m_window, COLOR_PAIR(Markdown::Colors::PAIR_ITALIC) | A_ITALIC);
  mvwprintw(m_window, subRow, subCol - 4, "%s", heading.c_str());
  wattroff(m_window, COLOR_PAIR(Markdown::Colors::PAIR_ITALIC) | A_ITALIC);
  if (m_notesWindow) {
    delwin(m_notesWindow);
  }
  m_notesWindow = derwin(m_window, subHeight, range, subRow + 1, margin);
  wattron(m_notesWindow, COLOR_PAIR(Markdown::Colors::PAIR_TEXT));
  for (int i = 0; i < static_cast<int>(m_recentNotes.size()); i++) {
    if (i == m_notesIndex) {
      wattron(m_notesWindow, A_REVERSE);
    }
    mvwprintw(m_notesWindow, i + 1, 2, "%s", m_recentNotes[i].title.c_str());
    if (i == m_notesIndex) {
      wattroff(m_notesWindow, A_REVERSE);
    }
  }
  wattroff(m_notesWindow, COLOR_PAIR(Markdown::Colors::PAIR_TEXT));
  wrefresh(m_window);
  wrefresh(m_notesWindow);
}

void MainMenu::setRecentNotes(std::vector<Model::Note> notes) {
  m_recentNotes = std::move(notes);
}

void MainMenu::setNotesIndex(int index) { m_notesIndex = index; }

} // namespace QuickNotes::UI
