#pragma once

#include "objects.hpp"
#include "screen.hpp"
#include "texture.hpp"

#define MAX_BARREL_COUNT 20
#define MAX_TILE_COUNT 300
#define MAX_LADDER_COUNT 20

class World {

public:
  int width, height;
  Player player;

  Ladder *ladders;
  int ladder_count;

  Tile *tiles;
  int tile_count;

  Barrel *barrels;
  int barrel_count;

  TextureManager &textures;

  World(int w, int h, TextureManager &textures);
  ~World();

  void update(double dt);
  void gen_tile_line(int skip_front, int skip_back, int tiles_from_bot,
                     double dy, int ladder_on_id = 0, int ladder_parts = 0);
  Ladder *intersecting_ladder(DynamicObject *obj);
  void draw(Screen &screen);
};
