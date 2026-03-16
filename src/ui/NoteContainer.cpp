#include "ui/NoteContainer.hpp"
#include "config/Config.hpp"
#include "ui/NoteListWidget.hpp"
#include "ui/PreviewWidget.hpp"
#include "ui/SearchBar.hpp"
#include "ui/StatusBar.hpp"
#include "ui/Widget.hpp"
#include <memory>
#include <string_view>

namespace QuickNotes::UI {

static constexpr int MARGIN = 2;
static constexpr int STATUS_H = 1;
static constexpr int LEFT_SIDE_RATIO_DENOM = 4;

NoteContainer::NoteContainer(
    WINDOW *window, std::shared_ptr<const Config::Config> config
)
    : Widget(window), m_config(config), m_list(nullptr), m_preview(nullptr),
      m_searchBar(nullptr), m_statusBar(nullptr),
      m_listWindow(window, listRect(window)),
      m_previewWindow(window, previewRect(window)),
      m_statusWindow(window, statusBarRect(window)),
      m_searchWindow(window, searchBarRect(window)) {
  m_list = std::make_unique<NoteListWidget>(m_listWindow.get());
  m_preview = std::make_unique<PreviewWidget>(m_previewWindow.get(), m_config);
  m_statusBar = std::make_unique<StatusBar>(m_statusWindow.get());
  m_searchBar = std::make_unique<SearchBar>(m_searchWindow.get());
}

void NoteContainer::draw(Notes &notes, int selectedIndex) {
  clear();
  m_list->draw(notes, selectedIndex);
  if (!notes.empty()) {
    m_preview->draw(notes[selectedIndex]);
  }
  m_statusBar->setLabel(m_modeLabel);
  m_statusBar->draw();
  m_searchBar->draw();
  wnoutrefresh(m_window);
}

void NoteContainer::draw(
    Notes &results, int selectedIndex, std::string_view input
) {
  clear();
  m_list->draw(results, selectedIndex);
  if (!results.empty()) {
    m_preview->draw(results[selectedIndex]);
  }
  m_statusBar->setLabel(m_modeLabel);
  m_statusBar->draw();
  m_searchBar->setInputBuffer(input);
  m_searchBar->draw();
  wnoutrefresh(m_window);
}

void NoteContainer::draw() {
  // no-op: meaningful draw requires data from state
}

void NoteContainer::resize(WINDOW *window) {
  m_list.reset();
  m_preview.reset();
  m_window = window;
  m_listWindow = SubWindow(window, listRect(window));
  m_previewWindow = SubWindow(window, previewRect(window));
  m_list = std::make_unique<NoteListWidget>(m_listWindow.get());
  m_preview = std::make_unique<PreviewWidget>(m_previewWindow.get(), m_config);
}

void NoteContainer::setMode(std::string_view mode) { m_modeLabel = mode; }

Rect NoteContainer::listRect(WINDOW *w) {
  const int h = getmaxy(w);
  const int listW = getmaxx(w) / LEFT_SIDE_RATIO_DENOM;
  return {.yPos = 3, .xPos = 0, .height = h - 3 - STATUS_H, .width = listW};
}

Rect NoteContainer::previewRect(WINDOW *w) {
  const int h = getmaxy(w);
  const int listW = getmaxx(w) / LEFT_SIDE_RATIO_DENOM;
  const int previewW = getmaxx(w) - listW - (MARGIN * 2);
  return {0, listW + MARGIN, h - 1, previewW};
}

Rect NoteContainer::searchBarRect(WINDOW *w) {
  const int listW = getmaxx(w) / LEFT_SIDE_RATIO_DENOM;
  return {.yPos = 0, .xPos = 0, .height = 3, .width = listW};
}

Rect NoteContainer::statusBarRect(WINDOW *w) {
  const int width = getmaxx(w);
  const int height = getmaxy(w);
  return {.yPos = height - 1, .xPos = 0, .height = 1, .width = width};
}

} // namespace QuickNotes::UI
