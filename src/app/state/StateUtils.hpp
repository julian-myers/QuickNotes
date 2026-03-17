#pragma once

#include "config/Config.hpp"
#include <algorithm>
#include <vector>

namespace QuickNotes::App::State {

/// @brief Translates a raw key code into a state-local action enum value.
///
/// Walks keyMap looking for a Config::Action whose bound key matches the
/// pressed key. Returns the associated ActionEnum value on a match, or
/// @p none if no binding is found.
///
/// Intended to be called at the top of each state's handleInput() so that the
/// rest of the method can switch on a semantic action rather than raw key codes.
///
/// @tparam ActionEnum  The state-local enum class describing possible actions.
/// @param key          The raw ncurses key code from wgetch().
/// @param binds        The current key-binding configuration.
/// @param keyMap       Ordered list of (Config::Action, ActionEnum) pairs to
///                     search.
/// @param none         The ActionEnum sentinel to return when no binding matches.
/// @return The matched ActionEnum value, or @p none.
template <typename ActionEnum>
ActionEnum dispatchKey(
    int key,
    const Config::KeyBindsConfig &binds,
    const std::vector<std::pair<Config::Action, ActionEnum>> &keyMap,
    ActionEnum none
) {
  auto it = std::ranges::find_if(keyMap, [&](const auto &pair) {
    return key == binds.bindings.at(pair.first);
  });
  return (it != keyMap.end()) ? it->second : none;
}

} // namespace QuickNotes::App::State
