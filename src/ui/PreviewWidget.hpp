#pragma once

#include "Widget.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include <memory>

namespace QuickNotes::UI {

/// @brief Renders a note's markdown content into a scrollable ncurses pad.
///
/// Internally allocates a tall ncurses pad (PAD_HEIGHT rows) so that content
/// longer than the visible window can be scrolled without reallocation.
/// The caller advances the view by passing a scroll offset to padRefresh().
///
/// Rendering is performed by the Markdown pipeline: the note content is
/// tokenized by Lexer, parsed by Parser into an AST, then traversed by
/// Renderer which writes styled output to the pad.
///
/// Move-only: copying is disabled because the pad is an ncurses resource
/// with a unique owner.
///
/// @see Markdown::Renderer
/// @see Widget
class PreviewWidget : public Widget {

  public:
    /// @brief Constructs a PreviewWidget.
    /// @param window Pointer to the ncurses WINDOW used as the viewport.
    /// @param config Shared pointer to the application configuration (colors).
    explicit PreviewWidget(
        WINDOW *window, std::shared_ptr<const Config::Config> config
    );

    /// @brief Move constructor. Transfers pad ownership from @p other.
    /// @param other The PreviewWidget to move from.
    PreviewWidget(PreviewWidget &&other) noexcept;

    /// @brief Move assignment. Transfers pad ownership from @p other.
    /// @param other The PreviewWidget to move from.
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

    /// @brief Copy the visible portion of the pad to the screen.
    ///
    /// Wraps prefresh(). Call this after draw() to make changes visible.
    ///
    /// @param scrollOffset Number of pad rows to skip from the top.
    /// @param yMin         Top row of the destination region on screen.
    /// @param xMin         Left column of the destination region on screen.
    /// @param yMax         Bottom row of the destination region on screen.
    /// @param xMax         Right column of the destination region on screen.
    void padRefresh(int scrollOffset, int yMin, int xMin, int yMax, int xMax);

    /// @brief Total height of the internal pad in rows.
    ///
    /// Must be larger than the longest note that will ever be rendered.
    static constexpr int PAD_HEIGHT = 5000;

  private:
    std::shared_ptr<const Config::Config> m_config;
    WINDOW *m_pad;
    // int m_windowWidth, m_windowHeight, m_startRow, m_startColumn;
};
} // namespace QuickNotes::UI
