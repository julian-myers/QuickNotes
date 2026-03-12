#pragma once

#include <ncurses.h>

namespace QuickNotes::UI {

/// @brief Abstract class from which UI elements will inherit.
///
/// Enforces implementation of the draw() method while handleInput() and
/// resize() are optional overrides.
class Widget {

  public:
    /// @brief Constructs a widget.
    /// @param window a pointer to an instance of an ncurses WINDOW.
    explicit Widget(WINDOW *window) : m_window(window) {}

    /// @brief Deconstructor.
    virtual ~Widget() = default;

    /// @breif Pure virtual method that defines how the window should draw its
    /// contents.
    virtual void draw() = 0;

    /// @breif Virtual method where implementations should define how to handle
    /// user input.
    ///
    /// @param key integer key mapping.
    virtual bool handleInput(int key) { return false; }

    /// @brief Virtual method where implementations determine how a
    /// window/widget should be resized.
    ///
    /// @param window A pointer to an instance of an ncurses WINDOW.
    virtual void resize(WINDOW *window) { m_window = window; }

  protected:
    WINDOW *m_window;
};

} // namespace QuickNotes::UI
