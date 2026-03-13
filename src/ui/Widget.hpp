#pragma once

#include <ncurses.h>

namespace QuickNotes::UI {

/// @brief Abstract class for all UI components.
///
/// Widgets should be purely presentational and only own the rendering logic.
/// Concrete widgets receive data via setters before draw() is called.
///
/// @note Enforces implementation of the draw() method while handleInput() and
/// resize() are optional overrides.
///
/// @see Subwindow
/// @see DialogWidget
class Widget {

  public:
    /// @brief Constructs a widget.
    /// @param window Pointer to the ncurses window this renders into.
    explicit Widget(WINDOW *window) : m_window(window) {}

    /// @brief Virtual destructor.
    virtual ~Widget() = default;

    /// @brief Render the widget's contents to m_window.
    virtual void draw() = 0;

    /// @brief Respond to a terminal Resize event.
    ///
    /// The default implementation replaces m_window with the resized window.
    /// Subclasses that contain subwindows should override to recreate them as
    /// well.
    ///
    /// @param window Pointer to the resized ncurses WINDOW.
    virtual void resize(WINDOW *window) { m_window = window; }

  protected:
    WINDOW *m_window;

    static void roundedBox(WINDOW *win) {
      static const cchar_t ls = {0, L"│"};
      static const cchar_t rs = {0, L"│"};
      static const cchar_t ts = {0, L"─"};
      static const cchar_t bs = {0, L"─"};
      static const cchar_t tl = {0, L"╭"};
      static const cchar_t tr = {0, L"╮"};
      static const cchar_t bl = {0, L"╰"};
      static const cchar_t br = {0, L"╯"};
      wborder_set(win, &ls, &rs, &ts, &bs, &tl, &tr, &bl, &br);
    }
};

} // namespace QuickNotes::UI
