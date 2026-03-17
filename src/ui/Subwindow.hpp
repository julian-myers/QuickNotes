#pragma once

#include "Widget.hpp"
#include <cstdlib>
#include <ncurses.h>

namespace QuickNotes::UI {

/// @brief Axis-aligned rectangle used to position a SubWindow within its parent.
struct Rect {
    int yPos;   ///< Row offset from the top of the parent window.
    int xPos;   ///< Column offset from the left of the parent window.
    int height; ///< Height in terminal rows.
    int width;  ///< Width in terminal columns.
};

/// @brief RAII wrapper around an ncurses derived window (derwin).
///
/// Creates a child window via derwin() at construction and calls delwin()
/// at destruction. Move-only: copying is disabled because ncurses windows
/// cannot be shared between owners.
///
/// If derwin() fails (e.g. the requested geometry extends outside the parent),
/// the constructor prints a diagnostic and calls std::abort().
///
/// @see Widget
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

    void drawBorder() { Widget::roundedBox(m_window); };

  private:
    Rect m_rect;
    WINDOW *m_window;
};

} // namespace QuickNotes::UI
