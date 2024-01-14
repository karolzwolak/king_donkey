#pragma once

#include "objects.hpp"
#include "screen.hpp"
#include "texture.hpp"

#define MAX_BARREL_COUNT 20
#define MAX_TILE_COUNT 300
#define MAX_LADDER_COUNT 20

#define MIN_BARREL_SPAWN_DELAY 0.05
#define MAX_BARREL_SPAWN_DELAY 1.
#define BARREL_ABOUT_TO_SPAWN_TIME 0.7
#define BARREL_SPAWN_TIME 0.2

#define BARREL_SPAWNER_WIDTH 46
#define BARREL_SPAWNER_HEIGHT 32

#define BARREL_SPAWN_X_OFFSET 41
#define BARREL_SPAWN_Y_OFFSET 20

#define PLAYER_START_X 20
#define PLAYER_START_Y LOGICAL_SCREEN_HEIGHT - 2 * TILE_HEIGHT - PLAYER_HEIGHT

#define ELAPSED_X 5
#define ELAPSED_Y 5
#define ELAPSED_MAX_LEN 20

#define DONE_X LOGICAL_SCREEN_WIDTH - 140
#define DONE_Y 5

class World;

enum BarrelSpawnerState {
  WAITING,
  ABOUT_TO_SPAWN,
  SPAWNING,
};

double random_double(double min, double max);

class BarrelSpawner {
public:
  AnimatedTexture texture;
  Vector2 pos;
  BarrelSpawnerState state;
  double timer;
  double spawn_delay;

  int id_to_replace;

  static AnimatedTexture create_texture();

  BarrelSpawner(Vector2 pos);

  int get_fallen_off_id(World *world);

  double random_spawn_delay();
  void reset();
  bool check_can_spawn(World *world);
  void update(World *world, double dt);
  void draw(Screen &screen);
  void spawn(World *world);
};

class World {

public:
  double time_elapsed;

  int width, height;
  Player player;

  Ladder *ladders;
  int ladder_count;

  Tile *tiles;
  int tile_count;

  Barrel *barrels;
  int barrel_count;

  BarrelSpawner barrel_spawner;

  StaticTextureManager &textures;

  World(int w, int h, StaticTextureManager &textures);
  ~World();

  void gen_level();
  void reset_level();
  void update(double dt);
  void gen_tile_line(int skip_front, int skip_back, int tiles_from_bot,
                     double dy, int ladder_on_id = 0, int ladder_parts = 0);
  Ladder *intersecting_ladder(DynamicObject *obj);
  void draw(Screen &screen);
};
