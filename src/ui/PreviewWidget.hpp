#pragma once

#include "Widget.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/Subwindow.hpp"
#include <memory>

namespace QuickNotes::UI {

class PreviewWidget : public Widget {

  public:
    /// @brief Constructs a PreviewWidget.
    /// @param window reference to an ncurses window.
    explicit PreviewWidget(
        WINDOW *window, std::shared_ptr<const Config::Config> config
    );

    /// @brief draw the window and content.
    void draw() override;

    /// @brief draw the window and content.
    /// @param note The note to preview.
    void draw(const Model::Note &note);

  private:
    std::shared_ptr<const Config::Config> m_config;
    SubWindow m_innerWindow;
    // int m_windowWidth, m_windowHeight, m_startRow, m_startColumn;
};

} // namespace QuickNotes::UI
