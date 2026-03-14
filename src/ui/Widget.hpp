#pragma once

#include <ncurses.h>
#include <string_view>

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
    void clear() { werase(m_window); }

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

    void drawBorder() { roundedBox(m_window); }

    void drawHRule(int row) {
      mvwhline(m_window, row, 0, ACS_HLINE, getmaxx(m_window));
    }

    void drawHRule(int row, int margin) {
      mvwhline(
          m_window, row, margin, ACS_HLINE, getmaxx(m_window) - (margin * 2)
      );
    }

    void drawVRule(int col) {
      mvwvline(m_window, 0, col, ACS_VLINE, getmaxy(m_window));
    }

    /// @brief Prints text at the given position with no attributes.
    /// @param y Row to print at.
    /// @param x Column to print at.
    /// @param text The text to print.
    void print(int y, int x, std::string_view text) {
      mvwprintw(
          m_window, y, x, "%.*s", static_cast<int>(text.size()), text.data()
      );
    }

    /// @brief Prints text clipped to a maximum width, padding with spaces.
    ///
    /// Useful for list rows where overflow would corrupt adjacent content.
    ///
    /// @param y Row to print at.
    /// @param x Column to print at.
    /// @param text The text to print.
    /// @param width Maximum character width.
    void printClipped(int y, int x, std::string_view text, int width) {
      mvwprintw(m_window, y, x, "%-*.*s", width, width, text.data());
    }

    // -------------------------------------------------------------------------
    // Attribute helpers
    // -------------------------------------------------------------------------

    /// @brief Enables an ncurses attribute or color pair.
    /// @param attrs Attribute flags (e.g. A_BOLD, COLOR_PAIR(n)).
    void attrOn(attr_t attrs) { wattron(m_window, attrs); }

    /// @brief Disables an ncurses attribute or color pair.
    /// @param attrs Attribute flags to turn off.
    void attrOff(attr_t attrs) { wattroff(m_window, attrs); }

    // -------------------------------------------------------------------------
    // Dimension helpers
    // -------------------------------------------------------------------------

    /// @brief Returns the current width of the widget's window.
    int width() const { return getmaxx(m_window); }

    /// @brief Returns the current height of the widget's window.
    int height() const { return getmaxy(m_window); }
    WINDOW *m_window;
};

} // namespace QuickNotes::UI
