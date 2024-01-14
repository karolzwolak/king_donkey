#pragma once

#include "primitives.hpp"
#include "screen.hpp"
#include "texture.hpp"

class World;

const float GRAVITY = 500;
const float MAX_VELOCITY = 5000;

const float PLAYER_MOVE_VEL = 100;
const float BARREL_MOVE_VEL = 1.25 * PLAYER_MOVE_VEL;
const float JUMP_VELOCITY = 200;

const float CLIMB_VELOCITY = 35;

const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 8;

const int LADDER_WIDTH = 8;
const int LADDER_HEIGHT = 8;

const int PLAYER_WIDTH = 16;
const int PLAYER_HEIGHT = 16;

const int BARREL_WIDTH = 12;
const int BARREL_HEIGHT = 12;

const int CLIMB_THRESHOLD = 0.25 * TILE_HEIGHT;
const int COYOTE_DIST = CLIMB_THRESHOLD + 0.1;

enum PlayerState {
  IDLE,
  WALKING,
  JUMPING,
  FALLING,
  CLIMBING,
};

enum MoveDirection {
  DIR_LEFT,
  DIR_RIGHT,
  DIR_UP,
  DIR_DOWN,
  DIR_NONE,
};

MoveDirection opposite_direction(MoveDirection dir);

class RectObject {
public:
  /// Position is the left top corner
  Vector2 pos;
  int width, height;

  RectObject(Vector2 pos, int w, int h);

  double top();
  double bottom();
  double left();
  double right();

  double center_x();
  double center_y();

  /* void draw_simple(Screen *screen, SimpleTexture *texture); */
  /* void draw_animated(Screen *screen, AnimatedTexture *texture); */
  bool collides_with(RectObject *obj);
};

class StaticObject {
public:
  RectObject obj;
  SimpleTexture *texture;

  StaticObject(Vector2 pos, int w, int h, SimpleTexture *texture);
  StaticObject(StaticObject &other);
  void draw(Screen *screen);
};

class DynamicObject {
public:
  RectObject rect_obj;
  Vector2 velocity, acceleration;
  double horizontal_move_vel;
  double fall_dist;
  bool on_ground;
  bool coyote_on_ground;
  Orientation orientation;
  AnimatedTexture texture;

  void limit_velocity();

public:
  DynamicObject(Vector2 pos, int w, int h, double horizontal_move_vel,
                AnimatedTexture texture);

  void set_velocity(Vector2 v);
  void set_acceleration_x(float x);

  bool check_tile_collisions_x(World *world);
  bool check_out_of_bounds_x(World *world);
  bool check_tile_collisions_y(World *world, bool check_bounds = true);
  bool check_out_of_bounds_y(World *world);

  void horizontal_movement(MoveDirection dir, double dt);
  void vertical_movement(MoveDirection dir, double dt);

  void update(MoveDirection dir, World *world, double dt);
  void update_texture(double dt, int state = -1);
  void draw(Screen *screen);
};

class Ladder {

public:
  StaticObject static_obj;
  int parts;

  Ladder();
  Ladder(Vector2 pos, int parts, SimpleTexture *texture);
  static SimpleTexture create_texture();

  RectObject *get_rect();
  void draw(Screen &screen);
};

class Tile {

public:
  StaticObject static_obj;

  Tile(Vector2 pos, SimpleTexture *texture);
  Tile();

  static SimpleTexture create_texture();
  RectObject *get_rect();
  void draw(Screen &screen);
};

class Barrel {

public:
  bool fallen_off;

  DynamicObject dynamic_obj;
  MoveDirection move_direction;

  Barrel(Vector2 pos);
  Barrel();

  static AnimatedTexture create_texture();

  RectObject &get_rect();

  void start_moving(MoveDirection dir);
  void freeze();
  void update(World *world, double dt);
  void draw(Screen &screen);
};

class Player {
  MoveDirection move_direction;
  bool on_ladder;
  PlayerState state;

public:
  Ladder *ladder;
  DynamicObject dynamic_obj;
  Player(Vector2 pos);

  static AnimatedTexture create_texture();

  void update(World *world, double dt);
  void draw(Screen &screen);

  RectObject &get_rect();

  bool check_on_ladder(World *world);

  void get_on_ladder(Ladder *ladder);
  void get_off_ladder();
  void player_vertical_movement(double dt);

  void move(MoveDirection dir, bool key_down);
  void jump();
};
