#include "PreviewWidget.hpp"
#include "markdown/Lexer.hpp"
#include "markdown/Parser.hpp"
#include "markdown/Renderer.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <memory>

namespace QuickNotes::UI {

PreviewWidget::PreviewWidget(WINDOW *window,
                             std::shared_ptr<Config::Config> config)
    : Widget(window), m_config(config) {}

void PreviewWidget::draw(Model::Note &note) {
  wclear(m_window);
  auto tokens = Markdown::Lexer(note.content).tokenize();
  auto ast = Markdown::Parser(std::move(tokens)).parse();
  Markdown::Renderer renderer(m_window, *m_config);
  renderer.visit(*ast.root);
  wnoutrefresh(m_window);
}

void PreviewWidget::draw() {}

// int PreviewWidget::windowWidth() { return m_windowWidth; }
//
// int PreviewWidget::windowHeight() { return m_windowHeight; }
//
// int PreviewWidget::startingRow() { return m_startRow; }
//
// int PreviewWidget::startingColumn() { return m_startColumn; }

} // namespace QuickNotes::UI
