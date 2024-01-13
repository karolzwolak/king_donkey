#include "world.hpp"
#include "objects.hpp"
#include "screen.hpp"
#include "texture.hpp"
#include <cassert>

World::World(int w, int h, TextureManager &textures)
    : width(w), height(h), player(Vector2(0, 0), NULL), tiles(NULL),
      tile_count(0), ladders(NULL), ladder_count(0), barrels(NULL),
      barrel_count(0), textures(textures) {

  player = Player(Vector2(0, 0), &textures.player_texture);

  tiles = new Tile[MAX_TILE_COUNT];
  tile_count = 0;

  ladders = new Ladder[MAX_LADDER_COUNT];
  ladder_count = 0;
  barrels = new Barrel[MAX_LADDER_COUNT];
  barrel_count = 0;

  gen_tile_line(1, 1, 2, -0.5);
  gen_tile_line(1, 4, 14, 1.25, -1, 2);
  gen_tile_line(4, 0, 18, -0.25, -1, 7);
  gen_tile_line(5, 2, 28, 0.5, 9, 4);

  gen_tile_line(3, 20, 32, 0.5, 5, 2);
  gen_tile_line(26, 4, 32, -1.5, 6, 2);

  gen_tile_line(0, 5, 38, 0);

  barrels[barrel_count++] =
      Barrel(Vector2(60, 80), DIR_RIGHT, &textures.barrel_texture);
  barrels[barrel_count++] =
      Barrel(Vector2(0, 20), DIR_LEFT, &textures.barrel_texture);
}

World::~World() {
  delete[] tiles;
  delete[] ladders;
  delete[] barrels;
}
void World::gen_tile_line(int skip_front, int skip_back, int tiles_from_bot,
                          double dy, int ladder_on_id, int ladder_parts) {
  int len = LOGICAL_SCREEN_WIDTH / TILE_WIDTH - skip_front - skip_back;
  assert(tile_count + len < MAX_TILE_COUNT);

  double x = TILE_WIDTH * skip_front;
  double y = LOGICAL_SCREEN_HEIGHT - TILE_HEIGHT * tiles_from_bot;

  if (ladder_parts > 0) {
    if (ladder_on_id < 0)
      ladder_on_id = len + ladder_on_id;

    assert(ladder_count < MAX_LADDER_COUNT);
    double ladder_x = x + ladder_on_id * TILE_WIDTH + LADDER_WIDTH / 2.;
    double ladder_y = y + +dy * ladder_on_id;

    ladders[ladder_count++] = Ladder(Vector2(ladder_x, ladder_y), ladder_parts,
                                     &textures.ladder_texture);
  }

  for (int i = 0; i < len; i++) {
    tiles[tile_count++] = Tile(Vector2(x, y), &textures.tile_texture);
    x += TILE_WIDTH;
    y += dy;
  }
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
