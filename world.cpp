#include "world.hpp"
#include "objects.hpp"
#include "texture.hpp"

World::World(int w, int h)
    : width(w), height(h), player(Player(Vector2(10, 10))), tiles(NULL),
      tile_count(0), ladders(NULL), ladder_count(0), player_texture(NULL),
      barrel_texture(NULL), tile_texture(NULL), ladder_texture(NULL),
      barrels(NULL), barrel_count(0) {

  player_texture = new AnimatedTexture(16, 16);
  AnimationFrames player_run_frames =
      AnimationFrames(0, 17, 3, 0.1, 0, OR_RIGHT);
  player_texture->add_animation(0, player_run_frames);

  tile_texture = new SimpleTexture(60, 35, 16, 8);

  tiles = new Tile[100];
  tile_count = 0;

  ladders = new Ladder[100];
  ladder_count = 0;

  barrels = new Barrel[100];
  barrel_count = 0;

  int x = 0;
  int y = 200;
  for (int i = 0; i < 10; i++) {
    tiles[tile_count++] = Tile(Vector2(x, y));
    x += TILE_WIDTH;
    y -= CLIMB_THRESHOLD;
  }
  tiles[tile_count++] = Tile(Vector2(x, y - 16));

  ladders[ladder_count++] = Ladder(Vector2(100, 100), 16, 32);
  tiles[tile_count++] = Tile(Vector2(100, 100));

  barrels[barrel_count++] = Barrel(Vector2(100, 80), DIR_LEFT);
}

Ladder *World::intersecting_ladder(Object *obj) {
  for (int i = 0; i < ladder_count; i++) {
    if (obj->collides_with(&ladders[i].obj)) {
      return &ladders[i];
    }
  }
  return NULL;
}

void World::update(double dt) {
  player.update(this, dt);
  player_texture->update(dt);

  for (int i = 0; i < barrel_count; i++) {
    barrels[i].update(this, dt);
  }
}
void World::draw(Screen &screen) {
  for (int i = 0; i < tile_count; i++) {
    tiles[i].draw(screen, tile_texture);
  }
  /**/
  /* for (int i = 0; i < ladder_count; i++) { */
  /*   ladders[i].draw(screen); */
  /* } */
  /**/
  /* for (int i = 0; i < barrel_count; i++) { */
  /*   barrels[i].draw(screen); */
  /* } */
  player.draw(screen, player_texture);
}
