#include "PreviewWidget.hpp"
#include "markdown/Lexer.hpp"
#include "markdown/Parser.hpp"
#include "markdown/Renderer.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <memory>

namespace QuickNotes::UI {

PreviewWidget::PreviewWidget(
    WINDOW *window, std::shared_ptr<const Config::Config> config
)
    : Widget(window), m_config(config),
      m_innerWindow(window, {1, 1, getmaxy(window) - 2, getmaxx(window) - 2}) {}

void PreviewWidget::draw(const Model::Note &note) {
  clear();
  drawBorder();
  wclear(m_innerWindow.get());
  auto tokens = Markdown::Lexer(note.content).tokenize();
  auto ast = Markdown::Parser(std::move(tokens)).parse();
  Markdown::Renderer renderer(m_innerWindow.get(), *m_config);
  renderer.visit(*ast.root);
  wnoutrefresh(m_innerWindow.get());
  wnoutrefresh(m_window);
}

void PreviewWidget::draw() {}

} // namespace QuickNotes::UI
