#include "objects.hpp"
#include "texture.hpp"
#include "world.hpp"

int sign(float x) {
  if (x > 0)
    return 1;
  if (x < 0)
    return -1;
  return 0;
}

MoveDirection opposite_direction(MoveDirection dir) {
  switch (dir) {
  case LEFT:
    return RIGHT;
  case RIGHT:
    return LEFT;
  case UP:
    return DOWN;
  case DOWN:
    return UP;
  default:
    return NONE;
  }
}

Object::Object(Vector2 pos, int w, int h) {
  position = pos;
  width = w;
  height = h;
}

double Object::top() { return position.y; }
double Object::bottom() { return position.y + height; }
double Object::left() { return position.x; }
double Object::right() { return position.x + width; }

double Object::center_x() { return position.x + width / 2.; }
double Object::center_y() { return position.y + height / 2.; }

void Object::draw_simple(Screen *screen, SimpleTexture *texture) {
  screen->draw_atlas_texture(&texture->rect, position.x, position.y);
}

void Object::draw_animated(Screen *screen, AnimatedTexture *texture) {
  screen->draw_atlas_texture(texture->get_curr_frame(), position.x, position.y);
}

bool Object::collides_with(Object *obj) {
  return left() < obj->right() && right() > obj->left() &&
         top() < obj->bottom() && bottom() > obj->top();
}

Ladder::Ladder(Vector2 pos, int w, int h) : obj(Object(pos, w, h)) {}
Ladder::Ladder() : obj(Object(Vector2(0, 0), 0, 0)) {}

/* void Ladder::draw(Screen &screen) { obj.draw(&screen); } */

Tile::Tile() : obj(Object(Vector2(0, 0), 0, 0)) {}
Tile::Tile(Vector2 pos) : obj(Object(pos, TILE_WIDTH, TILE_HEIGHT)) {}

/* void Tile::draw(Screen &screen) { obj.draw(&screen); } */

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

      double elevation_dist = obj->bottom() - world->tiles[i].obj.top();
      if (velocity.y == 0 && elevation_dist > 0 &&
          elevation_dist <= CLIMB_THRESHOLD) {
        obj->position.y -= elevation_dist;
        return true;
      }

      bool collision_from_left = velocity.x > 0;
      if (collision_from_left)
        obj->position.x = world->tiles[i].obj.left() - obj->width;
      else {
        obj->position.x = world->tiles[i].obj.right();
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
        obj->position.y = world->tiles[i].obj.top() - obj->height;
        on_ground = true;
        coyote_on_ground = true;

      } else {
        obj->position.y = world->tiles[i].obj.bottom();
      }

      velocity.y = 0;

      return true;
    }
  }

  if (coyote_on_ground && fall_dist > COYOTE_DIST)
    coyote_on_ground = false;

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

  double y = pos.y;
  pos.y += velocity.y * dt + acceleration.y * 0.5 * dt * dt;

  if (!on_ground && velocity.y > 0)
    fall_dist += pos.y - y;
  else
    fall_dist = 0;
}

void Dynamic::update(Object &obj, MoveDirection dir, World *world, double dt) {
  horizontal_movement(obj.position, dir, dt);
  check_tile_collisions_x(&obj, world);

  vertical_movement(obj.position, dir, dt);
  check_tile_collisions_y(&obj, world);
}

Player::Player(Vector2 pos, int w, int h)
    : dynamic(Dynamic()), obj(Object(pos, w, h)), on_ladder(false),
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

Barrel::Barrel(Vector2 pos, int w, int h, MoveDirection dir)
    : dynamic(Dynamic()), obj(Object(pos, w, h)), move_direction(dir) {}

Barrel::Barrel(Vector2 pos, MoveDirection dir)
    : Barrel(pos, BARREL_WIDTH, BARREL_HEIGHT, dir){};

Barrel::Barrel() : Barrel(Vector2(0, 0), NONE){};

Player::Player(Vector2 pos) : Player(pos, PLAYER_WIDTH, PLAYER_HEIGHT){};

void Barrel::update(World *world, double dt) {
  bool was_on_ground = dynamic.on_ground;
  double fall_dist = dynamic.fall_dist;

  dynamic.update(obj, move_direction, world, dt);

  if (dynamic.on_ground && !was_on_ground && fall_dist > 1 + CLIMB_THRESHOLD) {
    move_direction = opposite_direction(move_direction);
  }
}

/* void Barrel::draw(Screen &screen) { obj.draw(&screen); } */

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
  obj.position.x = ladder->obj.center_x() - obj.width / 2.;
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

void Player::draw(Screen &screen, AnimatedTexture *texture) {
  obj.draw_animated(&screen, texture);
}

void Player::move(MoveDirection dir, bool key_down) {
  // if lifted different key, stop moving
  if (!key_down) {
    if (dir == move_direction) {
      move_direction = NONE;
    }
    return;
  }
  if (on_ladder && (dir == LEFT || dir == RIGHT))
    return;

  move_direction = dir;
}

void Player::jump() {
  if (!dynamic.coyote_on_ground)
    return;

  dynamic.on_ground = false;
  dynamic.coyote_on_ground = false;
  dynamic.velocity.y = -JUMP_VELOCITY;
}
