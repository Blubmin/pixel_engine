#pragma once

#include <string>

namespace arraiy {
class GameMode {
 public:
  GameMode();

  virtual void Init() = 0;
  virtual void Update() = 0;
};

class GameModeManager {
  template <typename Mode>
  static void RegisterMode(const std::string& identifier) {
    static_assert(std::is_base_of<GameMode, Mode>());
  }
};
}  // namespace arraiy
