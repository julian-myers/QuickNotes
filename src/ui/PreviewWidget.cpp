#include "PreviewWidget.hpp"
#include "config/Config.hpp"
#include "markdown/Colors.hpp"
#include "markdown/Lexer.hpp"
#include "markdown/Parser.hpp"
#include "markdown/Renderer.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <memory>

namespace QuickNotes::UI {

using ConfigPtr = std::shared_ptr<const Config::Config>;

PreviewWidget::PreviewWidget(WINDOW *window, ConfigPtr config)
    : Widget(window),
      m_config(config),
      m_pad(newpad(PAD_HEIGHT, getmaxx(window) - 2)) {}

PreviewWidget::PreviewWidget(PreviewWidget &&other) noexcept
    : Widget(other.m_window),
      m_config(std::move(other.m_config)),
      m_pad(other.m_pad) {
  other.m_pad = nullptr;
}

PreviewWidget &PreviewWidget::operator=(PreviewWidget &&other) noexcept {
  if (this != &other) {
    if (m_pad) delwin(m_pad);
    m_pad = other.m_pad;
    other.m_pad = nullptr;
    m_config = std::move(other.m_config);
  }
  return *this;
}

PreviewWidget::~PreviewWidget() {
  if (m_pad) delwin(m_pad);
}

void PreviewWidget::draw(const Model::Note &note) {
  wclear(m_pad);
  roundedBox(m_window);
  std::string noteTile = " " + note.title + ": ";
  attrOn(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);
  print(0, 2, noteTile.c_str());
  attrOff(COLOR_PAIR(Markdown::Colors::PAIR_BOLD) | A_BOLD | A_ITALIC);
  wnoutrefresh(m_window);
  auto tokens = Markdown::Lexer(note.content).tokenize();
  auto ast = Markdown::Parser(std::move(tokens)).parse();
  Markdown::Renderer renderer(m_pad, *m_config);
  renderer.visit(*ast.root);
}

void PreviewWidget::padRefresh(
    int scrollOffset, int yMin, int xMin, int yMax, int xMax
) {
  int padMinCol = 0;
  pnoutrefresh(m_pad, scrollOffset, padMinCol, yMin, xMin, yMax, xMax);
}

void PreviewWidget::draw() {}

} // namespace QuickNotes::UI
