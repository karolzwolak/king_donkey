#include "world.hpp"

World::World(int w, int h)
    : width(w), height(h), player(Player(Vector2(10, 10), 16, 32)), tiles(NULL),
      tile_count(0) {

  tiles = new Tile[100];
  tile_count = 0;

  int x = 0;
  for (int i = 0; i < 20; i++) {
    tiles[tile_count++] = Tile(Vector2(x, 200));
    x += TILE_WIDTH;
  }
  tiles[tile_count++] = Tile(Vector2(x, 200 - 16));
}

void World::update(double dt) { player.update(this, dt); }
void World::draw(Screen &screen) {
  for (int i = 0; i < tile_count; i++) {
    tiles[i].draw(screen);
  }
  player.draw(screen);
}
