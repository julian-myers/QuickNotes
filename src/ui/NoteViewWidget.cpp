#include "NoteViewWidget.hpp"
#include "config/Config.hpp"
#include "markdown/Lexer.hpp"
#include "markdown/Parser.hpp"
#include "markdown/Renderer.hpp"
#include "models/Notes.hpp"
#include "ncurses.h"
#include "ui/Widget.hpp"
#include <memory>

namespace QuickNotes::UI {

using ConfigPtr = std::shared_ptr<Config::Config>;

NoteViewWidget::NoteViewWidget(
    WINDOW *window, ConfigPtr config, Model::Note &note
)
    : Widget(window), m_config(config), m_note(note) {}

NoteViewWidget::~NoteViewWidget() {}

void NoteViewWidget::draw() {
  auto tokens = Markdown::Lexer(m_note.content).tokenize();
  auto ast = Markdown::Parser(std::move(tokens)).parse();
  Markdown::Renderer renderer(m_window, *m_config);
  renderer.visit(*ast.root);
}

} // namespace QuickNotes::UI
