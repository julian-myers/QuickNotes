#pragma once

#include "config/Config.hpp"
#include <ncurses.h>

namespace QuickNotes::Markdown::Colors {

// ---- Individual Colors ----

// headings
constexpr int COLOR_HEADING_1 = 16;
constexpr int COLOR_HEADING_2 = 17;
constexpr int COLOR_HEADING_3 = 18;
constexpr int COLOR_HEADING_4 = 19;
constexpr int COLOR_HEADING_5 = 20;
constexpr int COLOR_HEADING_6 = 21;
constexpr int COLOR_HEADING_BG = 22;

// emphasis
constexpr int COLOR_BOLD_FG = 23;
constexpr int COLOR_ITALIC_FG = 24;
constexpr int COLOR_BOLD_ITALIC_FG = 25;

// code
constexpr int COLOR_INLINE_CODE_FG = 26;
constexpr int COLOR_INLINE_CODE_BG = 27;
constexpr int COLOR_CODE_BLOCK_FG = 28;
constexpr int COLOR_CODE_BLOCK_BG = 29;

// block elements
constexpr int COLOR_HORIZONTAL_RULE_FG = 30;
constexpr int COLOR_BLOCK_QUOTE_LEFT_BORDER_FG = 31;

// list
constexpr int COLOR_LIST_BULLETS_FG = 32;
constexpr int COLOR_LIST_NUMBERS_FG = 33;

// tables
constexpr int COLOR_TABLE_BORDERS_FG = 34;
constexpr int COLOR_TABLE_HEADER_FG = 35;
constexpr int COLOR_TABLE_BODY_FG = 36;

// base
constexpr int COLOR_TEXT_FG = 37;
constexpr int COLOR_TEXT_BG = 38;

// misc
constexpr int COLOR_CARD_FG = 39;
constexpr int COLOR_CARD_BG = 40;

constexpr int COLOR_CARD_SELECTED_FG = 41;
constexpr int COLOR_CARD_SELECTED_BG = 42;

constexpr int COLOR_TAG_FG = 43;
constexpr int COLOR_TAG_BG = 44;

constexpr int COLOR_CARD_VISUAL_FG = 45;
constexpr int COLOR_CARD_VISUAL_BG = 46;

// ---- Pairs ----
// headings
constexpr int PAIR_HEADING_1 = 1;
constexpr int PAIR_HEADING_2 = 2;
constexpr int PAIR_HEADING_3 = 3;
constexpr int PAIR_HEADING_4 = 4;
constexpr int PAIR_HEADING_5 = 5;
constexpr int PAIR_HEADING_6 = 6;

// emphasis
constexpr int PAIR_BOLD = 7;
constexpr int PAIR_ITALIC = 8;
constexpr int PAIR_BOLD_ITALIC = 9;

// code
constexpr int PAIR_INLINE_CODE = 10;
constexpr int PAIR_CODE_BLOCK = 11;

// block elements
constexpr int PAIR_BLOCK_QUOTE_LEFT_BORDER = 12;
constexpr int PAIR_HORIZONTAL_RULE = 13;

// list
constexpr int PAIR_LIST_BULLETS = 14;
constexpr int PAIR_LIST_NUMBERS = 15;

// tables
constexpr int PAIR_TABLE_BORDERS = 16;
constexpr int PAIR_TABLE_HEADER = 17;
constexpr int PAIR_TABLE_BODY = 18;

// base
constexpr int PAIR_TEXT = 19;

constexpr int PAIR_CARD = 20;
constexpr int PAIR_CARD_SELECTED = 21;

constexpr int PAIR_TAG = 22;

constexpr int PAIR_CARD_VISUAL = 23;  // orange bg — for card title highlight
constexpr int PAIR_VISUAL_ACCENT = 24; // orange fg — for border and status bar

/// @brief Struct to wrap RGB in a type.
struct NCursesRGB {
    int r, g, b;
};

/// @brief Helper method to convert hex colors to RGB.
///
/// @param hex a reference to a hex color string.
/// @return instance of NCursesRGB
/// @see NCursesRGB
NCursesRGB hexToRGB(const std::string &hex);

/// @brief Initialize the color pairs for ncurses.
/// @param config a refrence to an instance of Config which contains the user's
/// set colors or the default colors.
/// @see Config::Config
void initColorPairs(const Config::Config &config);
} // namespace QuickNotes::Markdown::Colors
