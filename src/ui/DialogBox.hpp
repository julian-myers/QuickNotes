#pragma once

#include "ui/Subwindow.hpp"
#include "ui/Widget.hpp"
#include <ncurses.h>
#include <string_view>

namespace QuickNotes::UI {

/// @brief Base class for modal dialog widgets.
///
/// Handles centered SubWindow creation and the shared chrome common to all
/// dialogs: a border, a bold title row, and a horizontal divider. Concrete
/// subclasses call drawChrome() from their draw() implementation and then
/// render their specific content below the divider row.
///
/// @see AddNoteWidget
/// @see DeleteConfirmWidget
class DialogBox : public Widget {
  public:
    /// @brief Constructs a DialogWidget centered within the parent window.
    /// @param parent Pointer to the parent ncurses WINDOW.
    /// @param height Dialog height in terminal rows.
    /// @param width Dialog width in terminal columns.
    DialogBox(WINDOW *parent, int height, int width);

    /// @brief Pure virtual draw — subclasses call drawChrome() then render
    /// their content.
    void draw() override = 0;

    /// @brief Recreates the centered SubWindow after a terminal resize.
    /// @param window Pointer to the resized parent WINDOW.
    void resize(WINDOW *window) override;

  protected:
    SubWindow m_dialog;

    /// @brief Draws the dialog border, bold title, and horizontal divider.
    /// @param title Text displayed in the title row.
    void drawChrome(std::string_view title);

  private:
    int m_height;
    int m_width;

    /// @brief Computes a centered Rect for the given dimensions within parent.
    static Rect centeredRect(WINDOW *parent, int height, int width);
};

} // namespace QuickNotes::UI
