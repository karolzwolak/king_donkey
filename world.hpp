#pragma once

#include "objects.hpp"

class World {
  int width, height;

public:
  Player player;

  World(int w, int h);

  void update(double dt);
  void draw(Screen &screen);
};
