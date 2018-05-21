#pragma once

#include <string>

namespace pixel_engine {
class Game {
  public:
    Game(const std::string& game_name);

  private:
    std::string game_name_;
};
}