#include "objects.hpp"

int sign(float x) {
  if (x > 0)
    return 1;
  if (x < 0)
    return -1;
  return 0;
}

Object::Object(Vector2 pos, int w, int h, SDL_Surface *s) {
  position = pos;
  width = w;
  height = h;
  sprite = s;
}

void Object::draw(Screen *screen) {
  /* screen->draw_sprite(sprite, position.x, position.y); */
  screen->draw_rect(position.x, position.y, width, height, 0x000000, 0x00ff00);
}

Tile::Tile() : obj(Object(Vector2(0, 0), 0, 0, NULL)) {}
Tile::Tile(Vector2 pos) : obj(Object(pos, TILE_WIDTH, TILE_HEIGHT, NULL)) {}

void Tile::draw(Screen &screen) { obj.draw(&screen); }

Dynamic::Dynamic() {
  velocity = Vector2(0, 0);
  acceleration = Vector2(0, GRAVITY);
}

void Dynamic::limit_velocity() {
  if (abs(velocity.x) > MAX_VELOCITY)
    velocity.x = sign(velocity.x) * MAX_VELOCITY;
  if (abs(velocity.y) > MAX_VELOCITY)
    velocity.y = sign(velocity.y) * MAX_VELOCITY;
}

void Dynamic::set_velocity(Vector2 v) { velocity = v; }

void Dynamic::set_acceleration_x(float x) { acceleration.x = x; }

// TODO: add collisions
void Dynamic::update(Vector2 *pos, bool on_ground, MoveDirection move_direction,
                     float dt) {
  /* acceleration.x -= velocity.x * FRICTION * dt; */
  velocity.x = 0;

  if (move_direction == LEFT) {
    velocity.x = -MOVE_VELOCITY;
  } else if (move_direction == RIGHT) {
    velocity.x = MOVE_VELOCITY;
  }

  velocity = velocity.add(acceleration.mul(dt));
  limit_velocity();

  float old_pos_y = pos->y;
  *pos = pos->add(velocity.mul(dt));
  *pos = pos->add(acceleration.mul(0.5 * dt * dt));

  /* if (on_ground && pos->y > old_pos_y) { */
  /*   pos->y = old_pos_y; */
  /* } */
}

Player::Player(Vector2 pos, int w, int h)
    : dynamic(Dynamic()), obj(Object(pos, w, h, NULL)), curr_ladder(NULL),
      on_ladder(false), move_direction(NONE) {}

Player::Player(Vector2 pos) : Player(pos, PLAYER_WIDTH, PLAYER_HEIGHT){};

void Player::update(double dt) {
  bool on_ground = obj.position.y == 0;
  dynamic.update(&obj.position, on_ground, move_direction, dt);
}

void Player::draw(Screen &screen) { obj.draw(&screen); }

void Player::move(MoveDirection dir) {
  printf("velocity: x: %f; y %f\n", dynamic.velocity.x, dynamic.velocity.y);
  printf("acceleration: x: %f; y %f\n", dynamic.acceleration.x,
         dynamic.acceleration.y);

  move_direction = dir;

  /* dynamic.set_acceleration_x(0); */
  /* dynamic.velocity.x = 0; */
  /* switch (dir) { */
  /* case LEFT: */
  /*     dynamic.velocity.x = -MOVE */
  /*   break; */
  /* case RIGHT: */
  /*     dynamic.velocity.x = 100; */
  /*   break; */
  /*     case NONE: */
  /*     dynamic.set_acceleration_x(0); */
  /*     dynamic.velocity.x = 0; */
  /*   break; */
  /* case UP: */
  /*   break; */
  /* case DOWN: */
  /*   break; */
  /* }; */
}
