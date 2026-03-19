#pragma once

#include <string>
#include <unordered_map>

namespace QuickNotes::Config {

/// @brief Color settings for every markdown and UI element.
///
/// Each field holds a hex color string (e.g. "#CBA6F7"). Default values are
/// the Catppuccin Mocha palette. Fields are grouped by the element they style:
/// headings, emphasis, code, block elements, lists, tables, and base text.
///
/// Loaded from the TOML config file by ConfigLoader; falls back to these
/// defaults if the file is absent or a key is missing.
///
/// @see Config
/// @see ConfigLoader
struct ColorConfig {
    // headings
    std::string heading1fg = "#CBA6F7";
    std::string heading2fg = "#F38BA8";
    std::string heading3fg = "#EBA0AC";
    std::string heading4fg = "#FAB387";
    std::string heading5fg = "#F9E2AF";
    std::string heading6fg = "#A6E3A1";
    std::string headingBackground = "#1E1E2E";

    // emphasis
    std::string boldfg = "#89B4FA";
    std::string italicfg = "#74C7EC";
    std::string boldItalicfg = "#F5C2E7";

    // code
    std::string inLineCodefg = "#A6ADC8";
    std::string inLineCodebg = "#181825";
    std::string codeBlockfg = "#CDD6F4";
    std::string codeBlockbg = "#181825";

    // block elements
    std::string horizontalRulefg = "#9399B2";
    std::string blockQuoteLeftBorderfg = "#A6ADC8";

    // lists
    std::string listBulletsfg = "#FAB387";
    std::string listNumberfg = "#FAB387";

    // tables
    std::string tableBorders = "#FAB387";
    std::string tableHeaderfg = "#B4BEFE";
    std::string tableBodyfg = "#CDD6F4";

    // base
    std::string textBackground = "#1E1E2E";
    std::string textForeground = "#CDD6F4";

    // misc
    std::string subtext1 = "#bac2de";
    std::string subtext0 = "#a6adc8";
    std::string overlay2 = "#9399b2";
    std::string overlay1 = "#7f849c";
    std::string overlay0 = "#6c7086";
    std::string surface2 = "#585b70";
    std::string surface1 = "#45475a";
    std::string surface0 = "#313244";
    std::string base = "#1e1e2e";
    std::string mantle = "#181825";
    std::string crust = "#11111b";
};

/// @brief Semantic actions that can be bound to keys in KeyBindsConfig.
///
/// States map raw ncurses key codes to Action values via KeyBindsConfig::bindings
/// so that all key-handling logic operates on named actions rather than
/// hard-coded key codes. This makes the keybinds fully remappable.
///
/// @see KeyBindsConfig
enum class Action {
  // navigation
  MOVE_UP,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
  SELECT,
  ESCAPE,

  // viewing note
  VIEW_EDIT,

  // main menu
  MAIN_MENU_NOTES,
  MAIN_MENU_SETTINGS,
  MAIN_MENU_QUIT,

  // notes
  NEW_NOTE,
  DELETE_NOTE,
  OPEN_NOTE,
  RENAME_NOTE,

  // app
  QUIT,
  SAVE,
  SEARCH,
  YES,
  NO,
  BACKSPACE,
};

/// @brief Maps each semantic Action to its ncurses key code.
///
/// Default bindings follow a Vim-style layout (hjkl navigation, q to quit,
/// etc.). ConfigLoader overwrites individual entries when the user's TOML
/// config specifies custom bindings.
///
/// @see Action
struct KeyBindsConfig {
    std::unordered_map<Action, int> bindings = {
        {Action::MOVE_UP, 'k'},
        {Action::MOVE_DOWN, 'j'},
        {Action::MOVE_RIGHT, 'l'},
        {Action::MOVE_LEFT, 'h'},
        {Action::VIEW_EDIT, 'e'},
        {Action::MAIN_MENU_NOTES, 'n'},
        {Action::MAIN_MENU_SETTINGS, 's'},
        {Action::MAIN_MENU_QUIT, 'q'},
        {Action::NEW_NOTE, 'a'},
        {Action::DELETE_NOTE, 'd'},
        {Action::SELECT, '\n'},
        {Action::RENAME_NOTE, 'r'},
        {Action::QUIT, 'q'},
        {Action::SAVE, 's'},
        {Action::SEARCH, '/'},
        {Action::ESCAPE, 27},
        {Action::YES, 'y'},
        {Action::NO, 'n'},
        {Action::BACKSPACE, 127},
    };
};

/// @brief Top-level configuration object passed throughout the application.
///
/// Aggregates ColorConfig and KeyBindsConfig. Constructed by ConfigLoader::load()
/// and shared as a std::shared_ptr<const Config> so that all components see
/// the same immutable snapshot for the lifetime of the application.
///
/// @see ColorConfig
/// @see KeyBindsConfig
/// @see ConfigLoader
struct Config {
    ColorConfig colors;
    KeyBindsConfig keyBinds;
};

} // namespace QuickNotes::Config
