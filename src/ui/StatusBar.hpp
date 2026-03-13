#pragma once

#include "ui/Widget.hpp"
#include <string>
#include <string_view>
namespace QuickNotes::UI {

/// @brief Single-row status bar rendered at the bottom of a window.
///
/// Displays a mode label and an optional input buffer. Purely presentational —
/// the owning state is responsible for mapping its internal mode to a display
/// label before calling draw().
class StatusBar : public Widget {
  public:
    /// @brief Constructs a StatusBar.
    /// @param window Pointer to the ncurses WINDOW to render into.
    explicit StatusBar(WINDOW *window);

    /// @brief Renders the status bar into the bottom row of m_window.
    void draw() override;

    /// @brief Sets the mode label displayed on the left side.
    /// @param label e.g. "--- NORMAL ----", ....
    void setLabel(std::string_view label);

    /// @brief Sets the input buffer displayed after the label.
    /// @param buffer Current user input, empty in NORMAL mode.
    void setInputBuffer(std::string_view buffer);

  private:
    std::string m_label;
    std::string m_inputBuffer;
};

} // namespace QuickNotes::UI
