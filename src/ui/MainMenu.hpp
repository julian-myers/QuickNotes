#pragma once

#include "Widget.hpp"
#include "models/Notes.hpp"
#include <array>
#include <vector>

namespace QuickNotes::UI {

/// @brief The main menu widget.
///
/// Displays Tile, three options: notes, settings, quit, and then a list of
/// recently opened notes.
///
/// @see Widget
/// @see MenuAction
class MainMenu : public Widget {

  public:
    /// @brief Constructs a MainMenu.
    explicit MainMenu(WINDOW *window);

    /// @brief Draws menu to window.
    void draw() override;

  private:
    static constexpr const char *LINE_1 = R"(
 _______  __   __  ___   _______  ___   _    
)";
    static constexpr const char *LINE_2 = R"(
|       ||  | |  ||   | |       ||   | | |   
)";
    static constexpr const char *LINE_3 = R"(
|   _   ||  | |  ||   | |       ||   |_| |   
)";
    static constexpr const char *LINE_4 = R"(
|  | |  ||  |_|  ||   | |       ||      _|   
)";
    static constexpr const char *LINE_5 = R"(
|  |_|  ||       ||   | |      _||     |_    
)";
    static constexpr const char *LINE_6 = R"(
|      | |       ||   | |     |_ |    _  |   
)";
    static constexpr const char *LINE_7 = R"(
|____||_||_______||___| |_______||___| |_|   
)";
    static constexpr const char *LINE_8 = R"(
 __    _  _______  _______  _______  _______ 
)";
    static constexpr const char *LINE_9 = R"(
|  |  | ||       ||       ||       ||       |
)";
    static constexpr const char *LINE_10 = R"(
|   |_| ||   _   ||_     _||    ___||  _____|
)";
    static constexpr const char *LINE_11 = R"(
|       ||  | |  |  |   |  |   |___ | |_____ 
)";
    static constexpr const char *LINE_12 = R"(
|  _    ||  |_|  |  |   |  |    ___||_____  |
)";
    static constexpr const char *LINE_13 = R"(
| | |   ||       |  |   |  |   |___  _____| |
)";
    static constexpr const char *LINE_14 = R"(
|_|  |__||_______|  |___|  |_______||_______|
)";

    static constexpr std::array<const char *, 14> TITLE = {
        LINE_1, LINE_2, LINE_3,  LINE_4,  LINE_5,  LINE_6,  LINE_7,
        LINE_8, LINE_9, LINE_10, LINE_11, LINE_12, LINE_13, LINE_14};
    static constexpr int TITLE_HEIGHT = 14;
    int m_winHeight, m_winWidth, m_titleWidth, m_startRow, m_startCol;
    std::vector<Model::Note> m_recentNotes;
    int m_notesIndex;
    WINDOW *m_notesWindow = nullptr;

    void drawTitle();
    std::tuple<int, int, int> drawOptions();
    void drawRecentNotes(int optionRow, int range, int margin);
};

} // namespace QuickNotes::UI
