#include "markdown/Colors.hpp"
#include <string>

namespace QuickNotes::Markdown::Colors {

NCursesRGB hexToRGB(const std::string &hex) {
  constexpr int SCALE_UP_FACTOR = 1000;
  constexpr int SCALE_DOWN_FACTOR = 255;
  constexpr int BASE = 16;
  constexpr int R_POS = 0;
  constexpr int G_POS = 2;
  constexpr int B_POS = 4;
  constexpr int SUBSTRING_LENGTH = 2;
  std::string h = hex[0] == '#' ? hex.substr(1) : hex;
  int r = std::stoi(h.substr(R_POS, SUBSTRING_LENGTH), nullptr, BASE);
  int g = std::stoi(h.substr(G_POS, SUBSTRING_LENGTH), nullptr, BASE);
  int b = std::stoi(h.substr(B_POS, SUBSTRING_LENGTH), nullptr, BASE);
  return {(r * SCALE_UP_FACTOR) / SCALE_DOWN_FACTOR,
          (g * SCALE_UP_FACTOR) / SCALE_DOWN_FACTOR,
          (b * SCALE_UP_FACTOR) / SCALE_DOWN_FACTOR};
}

void initColorPairs(const Config::Config &config) {
  auto defineColor = [&](int colorID, const std::string &hex) {
    auto [r, g, b] = hexToRGB(hex);
    init_color(colorID, r, g, b);
  };
  defineColor(COLOR_HEADING_1, config.colors.heading1fg);
  defineColor(COLOR_HEADING_2, config.colors.heading2fg);
  defineColor(COLOR_HEADING_3, config.colors.heading3fg);
  defineColor(COLOR_HEADING_4, config.colors.heading4fg);
  defineColor(COLOR_HEADING_5, config.colors.heading5fg);
  defineColor(COLOR_HEADING_6, config.colors.heading6fg);
  defineColor(COLOR_HEADING_BG, config.colors.headingBackground);
  defineColor(COLOR_BOLD_FG, config.colors.boldfg);
  defineColor(COLOR_ITALIC_FG, config.colors.italicfg);
  defineColor(COLOR_BOLD_ITALIC_FG, config.colors.boldItalicfg);
  defineColor(COLOR_INLINE_CODE_FG, config.colors.inLineCodefg);
  defineColor(COLOR_INLINE_CODE_BG, config.colors.inLineCodebg);
  defineColor(COLOR_CODE_BLOCK_FG, config.colors.codeBlockfg);
  defineColor(COLOR_CODE_BLOCK_BG, config.colors.codeBlockbg);
  defineColor(COLOR_HORIZONTAL_RULE_FG, config.colors.horizontalRulefg);
  defineColor(COLOR_BLOCK_QUOTE_LEFT_BORDER_FG,
              config.colors.blockQuoteLeftBorderfg);
  defineColor(COLOR_LIST_BULLETS_FG, config.colors.listBulletsfg);
  defineColor(COLOR_LIST_NUMBERS_FG, config.colors.listNumberfg);
  defineColor(COLOR_TABLE_BORDERS_FG, config.colors.tableBorders);
  defineColor(COLOR_TABLE_HEADER_FG, config.colors.tableHeaderfg);
  defineColor(COLOR_TABLE_BODY_FG, config.colors.tableBodyfg);
  defineColor(COLOR_TEXT_FG, config.colors.textForeground);
  defineColor(COLOR_TEXT_BG, config.colors.textBackground);

  init_pair(PAIR_HEADING_1, COLOR_HEADING_1, COLOR_HEADING_BG);
  init_pair(PAIR_HEADING_2, COLOR_HEADING_1, COLOR_HEADING_BG);
  init_pair(PAIR_HEADING_3, COLOR_HEADING_1, COLOR_HEADING_BG);
  init_pair(PAIR_HEADING_4, COLOR_HEADING_1, COLOR_HEADING_BG);
  init_pair(PAIR_HEADING_5, COLOR_HEADING_1, COLOR_HEADING_BG);
  init_pair(PAIR_HEADING_6, COLOR_HEADING_1, COLOR_HEADING_BG);
  init_pair(PAIR_BOLD, COLOR_BOLD_FG, COLOR_TEXT_BG);
  init_pair(PAIR_ITALIC, COLOR_ITALIC_FG, COLOR_TEXT_BG);
  init_pair(PAIR_BOLD_ITALIC, COLOR_BOLD_ITALIC_FG, COLOR_TEXT_BG);
  init_pair(PAIR_INLINE_CODE, COLOR_INLINE_CODE_FG, COLOR_INLINE_CODE_BG);
  init_pair(PAIR_CODE_BLOCK, COLOR_CODE_BLOCK_FG, COLOR_CODE_BLOCK_BG);
  init_pair(PAIR_BLOCK_QUOTE_LEFT_BORDER, COLOR_BLOCK_QUOTE_LEFT_BORDER_FG,
            COLOR_TEXT_BG);
  init_pair(PAIR_HORIZONTAL_RULE, COLOR_HORIZONTAL_RULE_FG, COLOR_TEXT_BG);
  init_pair(PAIR_LIST_BULLETS, COLOR_LIST_BULLETS_FG, COLOR_TEXT_BG);
  init_pair(PAIR_LIST_NUMBERS, COLOR_LIST_NUMBERS_FG, COLOR_TEXT_BG);
  init_pair(PAIR_TABLE_BORDERS, COLOR_TABLE_BORDERS_FG, COLOR_TEXT_BG);
  init_pair(PAIR_TABLE_HEADER, COLOR_TABLE_HEADER_FG, COLOR_TEXT_BG);
  init_pair(PAIR_TABLE_BODY, COLOR_TABLE_BODY_FG, COLOR_TEXT_BG);
  init_pair(PAIR_TEXT, COLOR_TEXT_FG, COLOR_TEXT_BG);
}
} // namespace QuickNotes::Markdown::Colors
