#include "objects.hpp"
#include "world.hpp"

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

bool Object::collides_with(Object *obj) {
  return position.x < obj->position.x + obj->width &&
         position.x + width > obj->position.x &&
         position.y < obj->position.y + obj->height &&
         position.y + height > obj->position.y;
}

Tile::Tile() : obj(Object(Vector2(0, 0), 0, 0, NULL)) {}
Tile::Tile(Vector2 pos) : obj(Object(pos, TILE_WIDTH, TILE_HEIGHT, NULL)) {}

void Tile::draw(Screen &screen) { obj.draw(&screen); }

Dynamic::Dynamic() {
  velocity = Vector2(0, 0);
  acceleration = Vector2(0, GRAVITY);
  on_ground = false;
}

void Dynamic::limit_velocity() {
  if (abs(velocity.x) > MAX_VELOCITY)
    velocity.x = sign(velocity.x) * MAX_VELOCITY;
  if (abs(velocity.y) > MAX_VELOCITY)
    velocity.y = sign(velocity.y) * MAX_VELOCITY;
}

void Dynamic::set_velocity(Vector2 v) { velocity = v; }

void Dynamic::set_acceleration_x(float x) { acceleration.x = x; }

bool Dynamic::check_tile_collisions_x(Object *obj, World *world) {
  for (int i = 0; i < world->tile_count; i++) {
    if (obj->collides_with(&world->tiles[i].obj)) {

      bool collision_from_left = velocity.x > 0;
      if (collision_from_left)
        obj->position.x = world->tiles[i].obj.position.x - obj->width;
      else {
        obj->position.x = world->tiles[i].obj.position.x + obj->width;
      }

      velocity.x = 0;

      return true;
    }
  }

  return false;
}

bool Dynamic::check_tile_collisions_y(Object *obj, World *world) {
  for (int i = 0; i < world->tile_count; i++) {
    if (obj->collides_with(&world->tiles[i].obj)) {

      bool collision_from_top = velocity.y > 0;
      if (collision_from_top) {
        obj->position.y = world->tiles[i].obj.position.y - obj->height;
        on_ground = true;

      } else {
        obj->position.y = world->tiles[i].obj.position.y + obj->height;
      }

      velocity.y = 0;

      return true;
    }
  }
  return false;
}

void Dynamic::horizontal_movement(Vector2 &pos, MoveDirection dir, double dt) {
  velocity.x = 0;

  if (dir == LEFT) {
    velocity.x = -MOVE_VELOCITY;
  } else if (dir == RIGHT) {
    velocity.x = MOVE_VELOCITY;
  }

  velocity.x += acceleration.x * dt;
  limit_velocity();

  pos.x += velocity.x * dt + acceleration.x * 0.5 * dt * dt;
}

void Dynamic::vertical_movement(Vector2 &pos, MoveDirection dir, double dt) {
  velocity.y += acceleration.y * dt;
  limit_velocity();

  pos.y += velocity.y * dt + acceleration.y * 0.5 * dt * dt;
}

void Dynamic::update(Object &obj, MoveDirection dir, World *world, double dt) {
  horizontal_movement(obj.position, dir, dt);
  check_tile_collisions_x(&obj, world);

  vertical_movement(obj.position, dir, dt);
  check_tile_collisions_y(&obj, world);
}

Player::Player(Vector2 pos, int w, int h)
    : dynamic(Dynamic()), obj(Object(pos, w, h, NULL)), curr_ladder(NULL),
      on_ladder(false), move_direction(NONE) {}

Player::Player(Vector2 pos) : Player(pos, PLAYER_WIDTH, PLAYER_HEIGHT){};

void Player::update(World *world, double dt) {
  dynamic.update(obj, move_direction, world, dt);
}

void Player::draw(Screen &screen) { obj.draw(&screen); }

void Player::move(MoveDirection dir, bool key_down) {
  // if lifted different key, stop moving
  if (!key_down) {
    if (dir == move_direction) {
      move_direction = NONE;
    }
    return;
  }
  move_direction = dir;
}

void Player::jump() {
  if (!dynamic.on_ground)
    return;

  dynamic.on_ground = false;
  dynamic.velocity.y = -JUMP_VELOCITY;
}
