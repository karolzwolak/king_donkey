#pragma once

#include "objects.hpp"

class World {
  int width, height;
  Tile *tiles;
  int tile_count;

public:
  Player player;

  World(int w, int h);

  void update(double dt);
  void draw(Screen &screen);
};
