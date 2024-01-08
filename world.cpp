#include "world.hpp"
#include "objects.hpp"

World::World(int w, int h)
    : width(w), height(h), player(Player(Vector2(10, 10))), tiles(NULL),
      tile_count(0), ladders(NULL), ladder_count(0) {

  tiles = new Tile[100];
  tile_count = 0;

  ladders = new Ladder[100];
  ladder_count = 0;

  int x = 0;
  for (int i = 0; i < 20; i++) {
    tiles[tile_count++] = Tile(Vector2(x, 200));
    x += TILE_WIDTH;
  }
  tiles[tile_count++] = Tile(Vector2(x, 200 - 16));

  ladders[ladder_count++] = Ladder(Vector2(100, 100), 16, 32);
  tiles[tile_count++] = Tile(Vector2(100, 100));
}

Ladder *World::intersecting_ladder(Object *obj) {
  for (int i = 0; i < ladder_count; i++) {
    if (obj->collides_with(&ladders[i].obj)) {
      return &ladders[i];
    }
  }
  return NULL;
}

void World::update(double dt) { player.update(this, dt); }
void World::draw(Screen &screen) {
  for (int i = 0; i < tile_count; i++) {
    tiles[i].draw(screen);
  }

  for (int i = 0; i < ladder_count; i++) {
    ladders[i].draw(screen);
  }
  player.draw(screen);
}
