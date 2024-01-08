#pragma once

#include "./SDL2-2.0.10/include/SDL.h"
#include "primitives.hpp"
#include "screen.hpp"

class World;

const float GRAVITY = 500;
const float MAX_VELOCITY = 5000;
const float MOVE_VELOCITY = 150;
const float JUMP_VELOCITY = 300;

const float CLIMB_VELOCITY = 50;

const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 16;

const int PLAYER_WIDTH = 16;
const int PLAYER_HEIGHT = 32;

enum PlayerState {
  IDLE,
  WALKING,
  JUMPING,
  FALLING,
  CLIMBING,
};

enum MoveDirection {
  LEFT,
  RIGHT,
  UP,
  DOWN,
  NONE,
};

class Object {
  SDL_Surface *sprite;

public:
  /// Position is the left top corner
  Vector2 position;
  int width, height;

  Object(Vector2 pos, int w, int h, SDL_Surface *s);

  double top();
  double bottom();
  double left();
  double right();

  double center_x();
  double center_y();

  void draw(Screen *screen);
  bool collides_with(Object *obj);
};

class Dynamic {
public:
  Vector2 velocity, acceleration;
  bool on_ground;

  void limit_velocity();

public:
  Dynamic();

  void set_velocity(Vector2 v);
  void set_acceleration_x(float x);

  bool check_tile_collisions_x(Object *obj, World *world);
  bool check_tile_collisions_y(Object *obj, World *world);

  void horizontal_movement(Vector2 &pos, MoveDirection dir, double dt);
  void vertical_movement(Vector2 &pos, MoveDirection dir, double dt);

  void update(Object &obj, MoveDirection dir, World *world, double dt);
};

class Ladder {

public:
  Object obj;

  Ladder();
  Ladder(Vector2 pos, int w, int h);
  void draw(Screen &screen);
};

class Tile {

public:
  Object obj;

  Tile();
  Tile(Vector2 pos);
  void draw(Screen &screen);
};

class Barell {
  Dynamic dynamic;
  Object obj;
};

class Player {
  Dynamic dynamic;
  Object obj;
  MoveDirection move_direction;
  bool on_ladder;
  /* PlayerState state; */
public:
  Player(Vector2 pos, int w, int h);

  Player(Vector2 pos);

  void update(World *world, double dt);
  void draw(Screen &screen);

  bool check_on_ladder(World *world);

  void get_on_ladder(Ladder *ladder);
  void get_off_ladder();
  void player_vertical_movement(double dt);

  void move(MoveDirection dir, bool key_down);
  void jump();
};
