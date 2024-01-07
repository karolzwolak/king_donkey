#pragma once

#include "objects.hpp"
#include "screen.hpp"

class World {
  int width, height;

public:
  Player player;

  Ladder *ladders;
  int ladder_count;

  Tile *tiles;
  int tile_count;

  World(int w, int h);

  void update(double dt);
  Ladder *intersecting_ladder(Object *obj);
  void draw(Screen &screen);
};
