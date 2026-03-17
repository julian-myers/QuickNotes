#include "config/Config.hpp"
#include <algorithm>
#include <vector>

namespace QuickNotes::App::State {

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
