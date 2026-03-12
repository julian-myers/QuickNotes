#pragma once

#include "Widget.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include <memory>

namespace QuickNotes::UI {

class PreviewWidget : public Widget {

  public:
    /// @brief Constructs a PreviewWidget.
    /// @param window reference to an ncurses window.
    explicit PreviewWidget(
        WINDOW *window, std::shared_ptr<Config::Config> config
    );

    /// @brief draw the window and content.
    void draw() override;

    /// @brief draw the window and content.
    /// @param note The note to preview.
    void draw(Model::Note &note);

    /// @brief Getter for the window with.
    /// @return Width of window
    // int windowWidth();
    // int windowHeight();

  private:
    std::shared_ptr<Config::Config> m_config;
    // int m_windowWidth, m_windowHeight, m_startRow, m_startColumn;
};

} // namespace QuickNotes::UI
