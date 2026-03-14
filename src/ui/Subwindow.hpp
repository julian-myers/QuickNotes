#pragma once

#include <cstdlib>
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
          m_rect(rect) {
      if (!m_window) {
        endwin();
        fprintf(
            stderr,
            "derwin failed: y=%d x=%d h=%d w=%d\n",
            rect.yPos,
            rect.xPos,
            rect.height,
            rect.width
        );
        std::abort();
      }
    }

    ~SubWindow() {
      if (m_window) delwin(m_window);
    }

    SubWindow(SubWindow &&other) noexcept
        : m_window(other.m_window), m_rect(other.m_rect) {
      other.m_window = nullptr;
    }

    SubWindow &operator=(SubWindow &&other) noexcept {
      if (this != &other) {
        if (m_window) delwin(m_window);
        m_window = other.m_window;
        m_rect = other.m_rect;
        other.m_window = nullptr;
      }
      return *this;
    }

    SubWindow(const SubWindow &) = delete;
    SubWindow &operator=(const SubWindow &) = delete;

    WINDOW *get() const { return m_window; }

    const Rect &rect() const { return m_rect; }

  private:
    Rect m_rect;
    WINDOW *m_window;
};

} // namespace QuickNotes::UI
