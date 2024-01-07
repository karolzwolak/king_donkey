#pragma once

#include "./SDL2-2.0.10/include/SDL.h"
#include "primitives.hpp"
#include "screen.hpp"

/* const float GRAVITY = 0; */
const float GRAVITY = 200;
const float MAX_VELOCITY = 5000;
/* const float FRICTION = 0.1; */
const float MOVE_VELOCITY = 150;

const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 16;

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
  Vector2 position;
  int width, height;

  Object(Vector2 pos, int w, int h, SDL_Surface *s);
  void draw(Screen *screen);
};

class Dynamic {
public:
  Vector2 velocity, acceleration;

  void limit_velocity();

public:
  Dynamic();

  void set_velocity(Vector2 v);
  void set_acceleration_x(float x);

  void update(Vector2 *pos, bool on_ground, MoveDirection move_direction,
              float dt);
};

class Ladder {
  int n_segments;
  Object bottom_piece;
};

class Tile {
  Object obj;

public:
  Tile();
  Tile(Vector2 pos);
  void draw(Screen &screen);
};

class Barell {
  Dynamic dynamic;
  Object obj;
};

const int PLAYER_WIDTH = 16;
const int PLAYER_HEIGHT = 32;

class Player {
  Dynamic dynamic;
  Object obj;
  Ladder *curr_ladder;
  MoveDirection move_direction;
  bool on_ladder;
  /* PlayerState state; */
public:
  Player(Vector2 pos, int w, int h);

  Player(Vector2 pos);

  void update(double dt);
  void draw(Screen &screen);

  void move(MoveDirection dir);
};
