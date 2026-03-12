#include "app/App.hpp"
#include "config/ConfigLoader.hpp"
#include "markdown/Lexer.hpp"
#include "markdown/Parser.hpp"
#include "markdown/Renderer.hpp"
#include <clocale>
#include <ncurses.h>

static constexpr const char *SAMPLE = R"(
# Heading 1

Some **bold** and *italic* text, and `inline code`.

## Heading 2

> A block quote with some content.

- First item
- Second item
- Third item

1. Ordered one
2. Ordered two

---

| Name  | Age |
| :---- | --: |
| Alice | 30  |
| Bob   | 25  |
```cpp
int main() { return 0; }
` ``
)";

int main() {
  // auto config = QuickNotes::Config::ConfigLoader::load();
  // auto tokens = QuickNotes::Markdown::Lexer(SAMPLE).tokenize();
  // auto ast = QuickNotes::Markdown::Parser(std::move(tokens)).parse();
  // setlocale(LC_ALL, "");
  // initscr();
  // cbreak();
  // noecho();
  // keypad(stdscr, TRUE);
  // QuickNotes::Markdown::Renderer renderer(stdscr, config);
  // renderer.visit(*ast.root);
  // refresh();
  // getch();
  // endwin();
  QuickNotes::App::App app;
  app.run();
  return 0;
}
