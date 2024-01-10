#pragma once

#include "objects.hpp"
#include "screen.hpp"
#include "texture.hpp"

class World {
  int width, height;

public:
  Player player;

  AnimatedTexture *player_texture;
  AnimatedTexture *barrel_texture;
  SimpleTexture *tile_texture;
  SimpleTexture *ladder_texture;

  Ladder *ladders;
  int ladder_count;

  Tile *tiles;
  int tile_count;

  Barrel *barrels;
  int barrel_count;

  World(int w, int h);
  ~World();

  void update(double dt);
  Ladder *intersecting_ladder(DynamicObject *obj);
  void draw(Screen &screen);
};
