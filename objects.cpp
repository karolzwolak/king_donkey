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

Ladder::Ladder(Vector2 pos, int w, int h) : obj(Object(pos, w, h, NULL)) {}
Ladder::Ladder() : obj(Object(Vector2(0, 0), 0, 0, NULL)) {}

void Ladder::draw(Screen &screen) { obj.draw(&screen); }

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
  on_ground = false;

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
    : dynamic(Dynamic()), obj(Object(pos, w, h, NULL)), on_ladder(false),
      move_direction(NONE) {}

void Player::player_vertical_movement(double dt) {
  if (on_ladder) {
    switch (move_direction) {
    case UP:
      dynamic.velocity.y = -CLIMB_VELOCITY;
      break;
    case DOWN:
      dynamic.velocity.y = CLIMB_VELOCITY;
      break;
    default:
      dynamic.velocity.y = 0;
      break;
    }
  }

  dynamic.vertical_movement(obj.position, move_direction, dt);
}

Player::Player(Vector2 pos) : Player(pos, PLAYER_WIDTH, PLAYER_HEIGHT){};

void Player::update(World *world, double dt) {
  dynamic.horizontal_movement(obj.position, move_direction, dt);
  if (!on_ladder)
    dynamic.check_tile_collisions_x(&obj, world);

  if (move_direction == UP && !on_ladder) {
    Ladder *ladder = world->intersecting_ladder(&obj);
    if (ladder != NULL) {
      get_on_ladder(ladder);
    }
  }

  player_vertical_movement(dt);

  if (on_ladder && world->intersecting_ladder(&obj) == NULL) {
    get_off_ladder();
  }

  if (!on_ladder || move_direction == DOWN) {
    dynamic.check_tile_collisions_y(&obj, world);
    // get off ladder early if there is ground above it
    if (on_ladder && dynamic.on_ground)
      get_off_ladder();
  }
}

void Player::get_on_ladder(Ladder *ladder) {
  on_ladder = true;
  obj.position.x = ladder->obj.position.x;
  dynamic.velocity.y = 0;
  dynamic.acceleration.y = 0;
  move_direction = NONE;
}

void Player::get_off_ladder() {
  on_ladder = false;
  dynamic.acceleration.y = GRAVITY;
  if (move_direction == UP)
    dynamic.velocity.y = -JUMP_VELOCITY / 4;
  else
    dynamic.velocity.y = 0;
  move_direction = NONE;
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
