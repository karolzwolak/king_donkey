#include "world.hpp"
#include "objects.hpp"
#include "texture.hpp"

World::World(int w, int h)
    : width(w), height(h), player(Vector2(0, 0), NULL), tiles(NULL),
      tile_count(0), ladders(NULL), ladder_count(0), player_texture(NULL),
      barrel_texture(NULL), tile_texture(NULL), ladder_texture(NULL),
      barrels(NULL), barrel_count(0) {

  player_texture = Player::create_texture();
  player = Player(Vector2(0, 0), player_texture);

  ladder_texture = Ladder::create_texture();
  tile_texture = Tile::create_texture();
  barrel_texture = Barrel::create_texture();

  tiles = new Tile[100];
  tile_count = 0;

  ladders = new Ladder[100];
  ladder_count = 0;
  barrels = new Barrel[100];
  barrel_count = 0;

  int x = 0;
  int y = 200;
  for (int i = 0; i < 10; i++) {
    tiles[tile_count++] = Tile(Vector2(x, y), tile_texture);
    x += TILE_WIDTH;
    y -= CLIMB_THRESHOLD;
  }
  tiles[tile_count++] = Tile(Vector2(x, y - 16), tile_texture);

  ladders[ladder_count++] = Ladder(Vector2(100, 100), 10, ladder_texture);
  tiles[tile_count++] = Tile(Vector2(100, 100), tile_texture);

  barrels[barrel_count++] = Barrel(Vector2(100, 80), DIR_LEFT, barrel_texture);
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
