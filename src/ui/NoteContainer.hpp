#pragma once

#include "Widget.hpp"
#include "config/Config.hpp"
#include "models/Notes.hpp"
#include "ui/NoteListWidget.hpp"
#include "ui/PreviewWidget.hpp"
#include "ui/SearchBar.hpp"
#include "ui/StatusBar.hpp"
#include "ui/Subwindow.hpp"
#include <memory>
#include <string_view>
#include <vector>

namespace QuickNotes::UI {

/// @brief Composite widget that owns the note-list, preview, search, and status panels.
///
/// Lays out four sub-panels within its parent window:
/// - Left panel: NoteListWidget showing the scrollable list of NoteCards.
/// - Right panel: PreviewWidget rendering the selected note's markdown.
/// - Bottom-left: SearchBar shown only in SEARCH mode.
/// - Bottom: StatusBar showing the current mode label.
///
/// NoteListState holds one NoteContainer and calls draw() / resize() /
/// scrollUp() / scrollDown() as the user interacts. The container forwards
/// method calls to the appropriate child widget.
///
/// @see NoteListState
/// @see NoteListWidget
/// @see PreviewWidget
/// @see Widget
class NoteContainer : public Widget {

  public:
    using Notes = const std::vector<Model::Note>;

    /// @brief Constructs a NoteContainer and creates all child sub-panels.
    /// @param window Pointer to the parent ncurses WINDOW.
    /// @param config Shared pointer to the application configuration.
    explicit NoteContainer(
        WINDOW *window, std::shared_ptr<const Config::Config> config
    );

    /// @brief Render the list and preview panels for the given notes.
    /// @param notes         The notes to display in the list.
    /// @param selectedIndex Index of the currently selected note.
    void draw(Notes &notes, int selectedIndex);

    /// @brief Render with an active search query shown in the search bar.
    /// @param notes         The filtered notes to display.
    /// @param selectedIndex Index of the currently selected note.
    /// @param input         The current search query string.
    void draw(Notes &notes, int selectedIndex, std::string_view input);

    /// @brief Render with no notes (clears all child panels).
    void draw() override;

    /// @brief Recreate all child sub-panels after a terminal resize.
    /// @param window Pointer to the resized parent WINDOW.
    void resize(WINDOW *window) override;

    /// @brief Update the mode label shown in the status bar.
    /// @param mode Display string for the current mode (e.g. "--- NORMAL ---").
    void setMode(std::string_view mode);

    /// @brief Scroll the preview panel up by one line.
    void scrollUp();

    /// @brief Scroll the preview panel down by one line.
    void scrollDown();

    /// @brief Reset the preview scroll position to the top.
    void resetScroll();

  private:
    enum class Mode { NORMAL, SEARCH, PREVIEW };

    std::string m_modeLabel = "---- NORMAL ----";
    Mode m_mode;
    std::shared_ptr<const Config::Config> m_config;
    std::unique_ptr<NoteListWidget> m_list;
    std::unique_ptr<PreviewWidget> m_preview;
    std::unique_ptr<StatusBar> m_statusBar;
    std::unique_ptr<SearchBar> m_searchBar;
    int m_previewScrollOffset = 0;

    SubWindow m_listWindow;
    SubWindow m_previewWindow;
    SubWindow m_statusWindow;
    SubWindow m_searchWindow;

    static Rect listRect(WINDOW *window);
    static Rect previewRect(WINDOW *window);
    static Rect searchBarRect(WINDOW *window);
    static Rect statusBarRect(WINDOW *window);

    void refreshPreview(const Model::Note &note);
};

} // namespace QuickNotes::UI
