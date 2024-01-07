#pragma once

#include "objects.hpp"
#include "screen.hpp"

class World {
  int width, height;

public:
  Player player;
  Tile *tiles;
  int tile_count;

  World(int w, int h);

  void update(double dt);
  void draw(Screen &screen);
};
