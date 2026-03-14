#include "ui/SearchBar.hpp"
#include "ui/Widget.hpp"
#include <string_view>

namespace QuickNotes::UI {

SearchBar::SearchBar(WINDOW *parent) : Widget(parent) {}

void SearchBar::setInputBuffer(std::string_view buffer) {
  m_inputBuffer = buffer;
}

void SearchBar::setLabel(std::string_view label) { m_label = label; }

void SearchBar::draw() {
  clear();
  int margin = 1;
  drawBorder();
  print(1, 2, m_inputBuffer);
  wnoutrefresh(m_window);
}

} // namespace QuickNotes::UI
