#include "./SDL2-2.0.10/include/SDL.h"
#include "primitives.cpp"

const float GRAVITY = -0.3;
const float MAX_VELOCITY = 5;
const float FRICTION = 0.1;

class Object {
  Vector2 position;
  int width, height;
  SDL_Surface *sprite;
};

int sign(float x) {
  if (x > 0)
    return 1;
  if (x < 0)
    return -1;
  return 0;
}

class Dynamic {
  Vector2 velocity, acceleration;

public:
  Dynamic() {
    velocity = Vector2(0, 0);
    acceleration = Vector2(0, GRAVITY);
  }

  void limit_velocity() {
    if (abs(velocity.x) > MAX_VELOCITY)
      velocity.x = sign(velocity.x) * MAX_VELOCITY;
    if (abs(velocity.y) > MAX_VELOCITY)
      velocity.y = sign(velocity.y) * MAX_VELOCITY;
  }

  void set_velocity(Vector2 v) { velocity = v; }

  void set_acceleration_x(float x) { acceleration.x = x; }

  // TODO: add collisions
  void update(Vector2 *pos, bool on_ground, float dt) {
    acceleration.x -= velocity.x * FRICTION;

    velocity = velocity.add(acceleration.mul(dt));
    limit_velocity();

    float old_pos_y = pos->y;
    *pos = pos->add(velocity.mul(dt));
    *pos = pos->add(acceleration.mul(0.5 * dt * dt));

    if (on_ground && pos->y < old_pos_y) {
      pos->y = old_pos_y;
    }
  }
};

class Ladder {
  int n_segments;
  Object bottom_piece;
};

class Tile {
  Object obj;
};

class Barell {
  Dynamic dynamic;
  Object obj;
};

enum PlayerState {
  IDLE,
  WALKING,
  JUMPING,
  FALLING,
  CLIMBING,
};

class Player {
  Dynamic dynamic;
  Object obj;
  Ladder *curr_ladder;
  bool on_ladder;
  /* PlayerState state; */
};
