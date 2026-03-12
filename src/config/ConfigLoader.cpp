#include "ConfigLoader.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <toml++/toml.hpp>

namespace QuickNotes::Config {

Config ConfigLoader::load() {
  auto path = configPath();
  if (!std::filesystem::exists(path)) {
    return defaults();
  }
  try {
    return parse(path);
  } catch (const toml::parse_error &e) {
    std::cerr << "Config parse error: " << e.description() << "\n";
    std::cerr << "Falling back to defaults. \n";
    return defaults();
  }
}

std::filesystem::path ConfigLoader::configPath() {
  const char *xdg = std::getenv("XDG_CONFIG_HOME");
  std::filesystem::path configDir =
      xdg ? std::filesystem::path(xdg)
          : std::filesystem::path(std::getenv("HOME")) / ".config";
  return configDir / "QuickNotes" / "config.toml";
}

Config ConfigLoader::defaults() { return Config{}; }

Config ConfigLoader::parse(const std::filesystem::path &path) {
  Config config;
  toml::table table = toml::parse_file(path.string());
  if (auto colors = table["colors"].as_table()) {
    auto get = [&](const std::string &key, std::string &field) {
      if (auto val = (*colors)[key].value<std::string>()) {
        field = *val;
      }
    };
    get("heading1", config.colors.heading1fg);
    get("heading2", config.colors.heading2fg);
    get("heading3", config.colors.heading3fg);
    get("heading4", config.colors.heading4fg);
    get("heading5", config.colors.heading5fg);
    get("heading6", config.colors.heading6fg);
    get("heading_bg", config.colors.headingBackground);
    get("bold_fg", config.colors.boldfg);
    get("italic_fg", config.colors.italicfg);
    get("bold_italic_fg", config.colors.boldItalicfg);
    get("in_line_code_fg", config.colors.inLineCodefg);
    get("in_line_code_bg", config.colors.inLineCodebg);
    get("code_block_fg", config.colors.codeBlockfg);
    get("code_block_bg", config.colors.codeBlockbg);
    get("horizontal_rule_fg", config.colors.horizontalRulefg);
    get("block_quote_left_border_fg", config.colors.blockQuoteLeftBorderfg);
    get("list_bullet_fg", config.colors.listBulletsfg);
    get("list_number_fg", config.colors.listNumberfg);
    get("table_border_fg", config.colors.tableBorders);
    get("table_header_fg", config.colors.tableHeaderfg);
    get("table_body_fg", config.colors.tableBodyfg);
    get("foreground", config.colors.textForeground);
    get("background", config.colors.textBackground);
  }
  return config;
}

} // namespace QuickNotes::Config
