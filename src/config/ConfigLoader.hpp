#pragma once

#include "Config.hpp"
#include <filesystem>

namespace QuickNotes::Config {

class ConfigLoader {
  public:
    static Config load();

  private:
    static std::filesystem::path configPath();
    static Config defaults();
    static Config parse(const std::filesystem::path &path);
};
} // namespace QuickNotes::Config
