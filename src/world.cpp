#include "world.hpp"
#include "objects.hpp"
#include "texture.hpp"

World::World(int w, int h, TextureManager &textures)
    : width(w), height(h), player(Vector2(0, 0), NULL), tiles(NULL),
      tile_count(0), ladders(NULL), ladder_count(0), barrels(NULL),
      barrel_count(0) {

  player = Player(Vector2(0, 0), &textures.player_texture);

  tiles = new Tile[100];
  tile_count = 0;

  ladders = new Ladder[100];
  ladder_count = 0;
  barrels = new Barrel[100];
  barrel_count = 0;

  tiles[tile_count++] = Tile(Vector2(0, 0), &textures.tile_texture);
  tiles[tile_count++] = Tile(Vector2(2, 2), &textures.tile_texture);
  tiles[tile_count++] = Tile(Vector2(10, 10), &textures.tile_texture);
  tiles[tile_count++] = Tile(Vector2(20, 20), &textures.tile_texture);
  tiles[tile_count++] =
      Tile(Vector2(640 - 16, 360 - 8), &textures.tile_texture);

  int x = 0;
  int y = 200;
  for (int i = 0; i < 10; i++) {
    tiles[tile_count++] = Tile(Vector2(x, y), &textures.tile_texture);
    x += TILE_WIDTH;
    y -= CLIMB_THRESHOLD;
  }

  for (int i = 0; i < 10; i++) {
    tiles[tile_count++] = Tile(Vector2(x, y), &textures.tile_texture);
    x += TILE_WIDTH;
  }

  ladders[ladder_count++] =
      Ladder(Vector2(100, 100), 10, &textures.ladder_texture);
  tiles[tile_count++] = Tile(Vector2(100, 100), &textures.tile_texture);

  barrels[barrel_count++] =
      Barrel(Vector2(10, 20), DIR_RIGHT, &textures.barrel_texture);
  barrels[barrel_count++] =
      Barrel(Vector2(0, 20), DIR_LEFT, &textures.barrel_texture);
}

World::~World() {
  delete[] tiles;
  delete[] ladders;
  delete[] barrels;
}

Ladder *World::intersecting_ladder(DynamicObject *obj) {
  for (int i = 0; i < ladder_count; i++) {
    if (obj->rect_obj.collides_with(ladders[i].get_rect())) {
      return &ladders[i];
    }
  }
  return NULL;
}

void World::update(double dt) {
  player.update(this, dt);

  for (int i = 0; i < barrel_count; i++) {
    barrels[i].update(this, dt);
  }
}

void World::draw(Screen &screen) {
  for (int i = 0; i < tile_count; i++) {
    tiles[i].draw(screen);
  }

  for (int i = 0; i < ladder_count; i++) {
    ladders[i].draw(screen);
  }

  for (int i = 0; i < barrel_count; i++) {
    barrels[i].draw(screen);
  }
  player.draw(screen);
}
