#include "world.hpp"
#include "objects.hpp"
#include "screen.hpp"
#include "texture.hpp"
#include <cassert>

double random_double(double min, double max) {
  return min + (double)rand() / RAND_MAX * (max - min);
}

World::World(int w, int h, StaticTextureManager &textures)
    : time_elapsed(0), width(w), height(h),
      player(Vector2(PLAYER_START_X, PLAYER_START_Y)), tiles(NULL),
      tile_count(0), ladders(NULL), ladder_count(0), barrels(NULL),
      barrel_count(0), textures(textures),
      barrel_spawner(
          BarrelSpawner(Vector2(5, LOGICAL_SCREEN_HEIGHT - 38 * TILE_HEIGHT -
                                       BARREL_SPAWNER_HEIGHT))) {
  gen_level();
}

void World::gen_level() {
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
}

void World::reset_level() {
  time_elapsed = 0;
  barrel_count = 0;
  player.dynamic_obj.reset(Vector2(PLAYER_START_X, PLAYER_START_Y));
  barrel_spawner.reset();
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
  time_elapsed += dt;

  player.update(this, dt);

  for (int i = 0; i < barrel_count; i++) {
    barrels[i].update(this, dt);
  }

  barrel_spawner.update(this, dt);
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
  barrel_spawner.draw(screen);

  char str[ELAPSED_MAX_LEN];
  sprintf(str, "Time: %.2fs", time_elapsed);
  screen.draw_string(ELAPSED_X, ELAPSED_Y, str);

  screen.draw_string(DONE_X, DONE_Y, "done: a, c, e, h");
}

BarrelSpawner::BarrelSpawner(Vector2 pos)
    : pos(pos), texture(BarrelSpawner::create_texture()), state(WAITING),
      timer(0), id_to_replace(-1), spawn_delay(MIN_BARREL_SPAWN_DELAY) {}

AnimatedTexture BarrelSpawner::create_texture() {
  AnimatedTexture texture(BARREL_SPAWNER_WIDTH, BARREL_SPAWNER_HEIGHT);
  texture.add_animation(WAITING, AnimationFrames(92, 15, 1, 0, 0, OR_NONE));

  texture.add_animation(ABOUT_TO_SPAWN,
                        AnimationFrames(46, 47, 2, 0.15, 0, OR_NONE));

  texture.add_animation(SPAWNING, AnimationFrames(0, 47, 1, 0, 0, OR_NONE));

  return texture;
}

void BarrelSpawner::reset() {
  state = WAITING;
  timer = 0;
  id_to_replace = -1;
}

bool BarrelSpawner::check_can_spawn(World *world) {
  if (id_to_replace < 0) {
    id_to_replace = get_fallen_off_id(world);
  }
  return id_to_replace >= 0 || world->barrel_count < MAX_BARREL_COUNT;
}

double BarrelSpawner::random_spawn_delay() {
  return random_double(MAX_BARREL_SPAWN_DELAY, MAX_BARREL_SPAWN_DELAY);
}

void BarrelSpawner::update(World *world, double dt) {
  timer += dt;
  texture.update(dt, true);

  switch (state) {
  case WAITING:
    if (timer >= spawn_delay && check_can_spawn(world)) {
      state = ABOUT_TO_SPAWN;
      timer = 0;
      spawn_delay = random_spawn_delay();
    }
    break;
  case ABOUT_TO_SPAWN:
    if (timer >= BARREL_ABOUT_TO_SPAWN_TIME) {
      state = SPAWNING;
      timer = 0;
      spawn(world);
    }
    break;
  case SPAWNING:
    if (timer >= BARREL_SPAWN_TIME) {
      state = WAITING;
      timer = 0;

      world->barrels[world->barrel_count - 1].start_moving(DIR_RIGHT);
    }
    break;
  };

  texture.change_state(state);
}

void BarrelSpawner::draw(Screen &screen) {
  texture.draw(&screen, pos.x, pos.y);
}

int BarrelSpawner::get_fallen_off_id(World *world) {
  for (int i = 0; i < world->barrel_count; i++) {
    if (world->barrels[i].fallen_off) {
      return i;
    }
  }
  return -1;
}

void BarrelSpawner::spawn(World *world) {
  Vector2 pos = Vector2(this->pos.x + BARREL_SPAWN_X_OFFSET,
                        this->pos.y + BARREL_SPAWN_Y_OFFSET);

  if (id_to_replace >= 0) {
    // if theres is barrel to replace (teleport), swap it with the last one
    // so the newest barrel is always at the end of the array
    world->barrels[id_to_replace] = world->barrels[world->barrel_count - 1];
    world->barrels[world->barrel_count - 1].get_rect().pos = pos;
    id_to_replace = -1;
    return;
  }
  world->barrels[world->barrel_count++].get_rect().pos = pos;
}
