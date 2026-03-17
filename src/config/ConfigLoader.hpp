#pragma once

#include "Config.hpp"
#include <filesystem>

namespace QuickNotes::Config {

/// @brief Loads the application configuration from a TOML file.
///
/// The config file is expected at a platform-appropriate path returned by
/// configPath() (typically ~/.config/quicknotes/config.toml). If the file
/// does not exist, defaults() is returned unchanged. If the file exists but
/// a key is absent, the default value for that key is preserved.
///
/// All public access is through the static load() factory — this class is
/// not meant to be instantiated.
///
/// @see Config
class ConfigLoader {
  public:
    /// @brief Load and return the application configuration.
    ///
    /// Reads the TOML config from configPath() if it exists, merges it with
    /// defaults(), and returns the result.
    ///
    /// @return A fully populated Config instance.
    static Config load();

  private:
    /// @brief Returns the path to the TOML configuration file.
    static std::filesystem::path configPath();

    /// @brief Returns a Config populated entirely with built-in default values.
    static Config defaults();

    /// @brief Parses a TOML file at @p path and returns the resulting Config.
    /// @param path Path to the TOML configuration file.
    static Config parse(const std::filesystem::path &path);
};
} // namespace QuickNotes::Config
