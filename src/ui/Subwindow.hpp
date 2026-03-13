#pragma once

#include <ncurses.h>

namespace QuickNotes::UI {

struct Rect {
    int yPos, xPos, height, width;
};

class SubWindow {
  public:
    SubWindow(WINDOW *parent, Rect rect)
        : m_window(
              derwin(parent, rect.height, rect.width, rect.yPos, rect.xPos)
          ),
          m_rect(rect) {}

    ~SubWindow() {
      if (m_window) delwin(m_window);
    }

    WINDOW *get() const { return m_window; }

    const Rect &rect() const { return m_rect; }

  private:
    Rect m_rect;
    WINDOW *m_window;
};

} // namespace QuickNotes::UI
