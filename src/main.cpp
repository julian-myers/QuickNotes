#include "app/App.hpp"
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
  QuickNotes::App::App app;
  app.run();
  return 0;
}
