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
        WINDOW *window, std::shared_ptr<const Config::Config> config
    );

    /// @brief Constructs a PreviewWidget by moving data from another instance.
    /// @param other Another PreviewWidget
    PreviewWidget(PreviewWidget &&other) noexcept;

    /// @brief Steals another PreviewWidget's belongings and then kills it.
    /// @param other Another PreviewWidget
    PreviewWidget &operator=(PreviewWidget &&other) noexcept;

    // Copying doesn't really make any sense here.
    // If we wanted to implement, it would have to be a deep copy which seems
    // useless.
    PreviewWidget(const PreviewWidget &) = delete;
    PreviewWidget &operator=(const PreviewWidget &) = delete;

    ~PreviewWidget();

    /// @brief draw the window and content.
    void draw() override;

    /// @brief draw the window and content.
    /// @param note The note to preview.
    void draw(const Model::Note &note);

    void padRefresh(int scrollOffset, int yMin, int xMin, int yMax, int xMax);

    static constexpr int PAD_HEIGHT = 5000;

  private:
    std::shared_ptr<const Config::Config> m_config;
    WINDOW *m_pad;
    // int m_windowWidth, m_windowHeight, m_startRow, m_startColumn;
};
} // namespace QuickNotes::UI
