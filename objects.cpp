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
  case DIR_LEFT:
    return DIR_RIGHT;
  case DIR_RIGHT:
    return DIR_LEFT;
  case DIR_UP:
    return DIR_DOWN;
  case DIR_DOWN:
    return DIR_UP;
  default:
    return DIR_NONE;
  }
}

RectObject::RectObject(Vector2 pos, int w, int h)
    : pos(pos), width(w), height(h) {}

double RectObject::top() { return pos.y; }
double RectObject::bottom() { return pos.y + height; }
double RectObject::left() { return pos.x; }
double RectObject::right() { return pos.x + width; }

double RectObject::center_x() { return pos.x + width / 2.; }
double RectObject::center_y() { return pos.y + height / 2.; }

/* void RectObject::draw_simple(Screen *screen, SimpleTexture *texture) { */
/*   screen->draw_atlas_texture(&texture->rect, pos.x, pos.y); */
/* } */
/**/
/* void RectObject::draw_animated(Screen *screen, AnimatedTexture *texture) { */
/*   texture->draw(screen, pos.x, pos.y); */
/* } */
/**/

bool RectObject::collides_with(RectObject *obj) {
  return left() < obj->right() && right() > obj->left() &&
         top() < obj->bottom() && bottom() > obj->top();
}

Ladder::Ladder(Vector2 pos, int w, int h, SimpleTexture *texture)
    : static_obj(pos, w, h, texture) {}
Ladder::Ladder() : Ladder(Vector2(0, 0), 0, 0, NULL) {}

RectObject *Ladder::get_rect() { return &static_obj.obj; }

StaticObject::StaticObject(Vector2 pos, int w, int h, SimpleTexture *texture)
    : obj(pos, w, h), texture(texture) {}

void StaticObject::draw(Screen *screen) {
  screen->draw_atlas_texture(&texture->rect, obj.pos.x, obj.pos.y);
}

/* void Ladder::draw(Screen &screen) { obj.draw(&screen); } */

Tile::Tile(Vector2 pos, SimpleTexture *texture)
    : static_obj(pos, TILE_WIDTH, TILE_HEIGHT, texture) {}

Tile::Tile() : Tile(Vector2(0, 0), NULL) {}

RectObject *Tile::get_rect() { return &static_obj.obj; }

void Tile::draw(Screen &screen) { static_obj.draw(&screen); }

DynamicObject::DynamicObject(Vector2 pos, int w, int h,
                             AnimatedTexture *texture)
    : rect_obj(pos, w, h), texture(texture), velocity(0, 0),
      acceleration(0, GRAVITY), on_ground(false), orientation(OR_NONE) {}

void DynamicObject::limit_velocity() {
  if (abs(velocity.x) > MAX_VELOCITY)
    velocity.x = sign(velocity.x) * MAX_VELOCITY;
  if (abs(velocity.y) > MAX_VELOCITY)
    velocity.y = sign(velocity.y) * MAX_VELOCITY;
}

void DynamicObject::set_velocity(Vector2 v) { velocity = v; }

void DynamicObject::set_acceleration_x(float x) { acceleration.x = x; }

bool DynamicObject::check_tile_collisions_x(World *world) {
  for (int i = 0; i < world->tile_count; i++) {
    RectObject *tile_rect = world->tiles[i].get_rect();
    if (rect_obj.collides_with(tile_rect)) {

      double elevation_dist = rect_obj.bottom() - tile_rect->top();
      if (velocity.y == 0 && elevation_dist > 0 &&
          elevation_dist <= CLIMB_THRESHOLD) {
        rect_obj.pos.y -= elevation_dist;
        return true;
      }

      bool collision_from_left = velocity.x > 0;
      if (collision_from_left)
        rect_obj.pos.x = tile_rect->left() - rect_obj.width;
      else {
        rect_obj.pos.x = tile_rect->right();
      }

      velocity.x = 0;

      return true;
    }
  }

  return false;
}

bool DynamicObject::check_tile_collisions_y(World *world) {
  on_ground = false;

  for (int i = 0; i < world->tile_count; i++) {

    RectObject *tile_rect = world->tiles[i].get_rect();
    if (rect_obj.collides_with(tile_rect)) {

      bool collision_from_top = velocity.y > 0;
      if (collision_from_top) {
        rect_obj.pos.y = tile_rect->top() - rect_obj.height;
        on_ground = true;
        coyote_on_ground = true;

      } else {
        rect_obj.pos.y = tile_rect->bottom();
      }

      velocity.y = 0;

      return true;
    }
  }

  if (coyote_on_ground && fall_dist > COYOTE_DIST)
    coyote_on_ground = false;

  return false;
}

void DynamicObject::horizontal_movement(MoveDirection dir, double dt) {
  velocity.x = 0;

  if (dir == DIR_LEFT) {
    velocity.x = -MOVE_VELOCITY;
    orientation = OR_LEFT;
  } else if (dir == DIR_RIGHT) {
    velocity.x = MOVE_VELOCITY;
    orientation = OR_RIGHT;
  }

  velocity.x += acceleration.x * dt;
  limit_velocity();

  rect_obj.pos.x += velocity.x * dt + acceleration.x * 0.5 * dt * dt;
}

void DynamicObject::vertical_movement(MoveDirection dir, double dt) {
  velocity.y += acceleration.y * dt;

  limit_velocity();

  double y = rect_obj.pos.y;
  rect_obj.pos.y += velocity.y * dt + acceleration.y * 0.5 * dt * dt;

  if (!on_ground && velocity.y > 0)
    fall_dist += rect_obj.pos.y - y;
  else
    fall_dist = 0;
}

void DynamicObject::update(MoveDirection dir, World *world, double dt) {
  horizontal_movement(dir, dt);
  check_tile_collisions_x(world);

  vertical_movement(dir, dt);
  check_tile_collisions_y(world);
}

void DynamicObject::draw(Screen *screen) {
  texture->change_orientation(orientation);
  texture->draw(screen, rect_obj.pos.x, rect_obj.pos.y);
}

Player::Player(Vector2 pos, AnimatedTexture *texture)
    : dynamic_obj(pos, PLAYER_WIDTH, PLAYER_HEIGHT, texture), on_ladder(false),
      move_direction(DIR_NONE) {}

void Player::player_vertical_movement(double dt) {
  if (on_ladder) {
    switch (move_direction) {
    case DIR_UP:
      dynamic_obj.velocity.y = -CLIMB_VELOCITY;
      break;
    case DIR_DOWN:
      dynamic_obj.velocity.y = CLIMB_VELOCITY;
      break;
    default:
      dynamic_obj.velocity.y = 0;
      break;
    }
  }

  dynamic_obj.vertical_movement(move_direction, dt);
}

Barrel::Barrel(Vector2 pos, MoveDirection dir, AnimatedTexture *texture)
    : dynamic_obj(pos, BARREL_WIDTH, BARREL_HEIGHT, texture),
      move_direction(dir) {}

Barrel::Barrel() : Barrel(Vector2(0, 0), DIR_NONE, NULL){};

/* Player::Player(Vector2 pos) : Player(pos, PLAYER_WIDTH, PLAYER_HEIGHT,
 * NULL){}; */

RectObject &Barrel::get_rect() { return dynamic_obj.rect_obj; }

void Barrel::update(World *world, double dt) {
  bool was_on_ground = dynamic_obj.on_ground;
  double fall_dist = dynamic_obj.fall_dist;

  dynamic_obj.update(move_direction, world, dt);

  if (dynamic_obj.on_ground && !was_on_ground &&
      fall_dist > 1 + CLIMB_THRESHOLD) {
    move_direction = opposite_direction(move_direction);
  }
}

/* void Barrel::draw(Screen &screen) { obj.draw(&screen); } */

RectObject &Player::get_rect() { return dynamic_obj.rect_obj; }

void Player::update(World *world, double dt) {
  dynamic_obj.horizontal_movement(move_direction, dt);
  if (!on_ladder)
    dynamic_obj.check_tile_collisions_x(world);

  if (move_direction == DIR_UP && !on_ladder) {
    Ladder *ladder = world->intersecting_ladder(&dynamic_obj);
    if (ladder != NULL) {
      get_on_ladder(ladder);
    }
  }

  player_vertical_movement(dt);

  if (on_ladder && world->intersecting_ladder(&dynamic_obj) == NULL) {
    get_off_ladder();
  }

  if (!on_ladder || move_direction == DIR_DOWN) {
    dynamic_obj.check_tile_collisions_y(world);
    // get off ladder early if there is ground above it
    if (on_ladder && dynamic_obj.on_ground)
      get_off_ladder();
  }
}

void Player::get_on_ladder(Ladder *ladder) {
  on_ladder = true;
  get_rect().pos.x = ladder->get_rect()->center_x() - get_rect().width / 2.;
  dynamic_obj.velocity.y = 0;
  dynamic_obj.acceleration.y = 0;
  move_direction = DIR_NONE;
}

void Player::get_off_ladder() {
  on_ladder = false;
  dynamic_obj.acceleration.y = GRAVITY;
  if (move_direction == DIR_UP)
    dynamic_obj.velocity.y = -JUMP_VELOCITY / 4;
  else
    dynamic_obj.velocity.y = 0;
  move_direction = DIR_NONE;
}

void Player::draw(Screen &screen) {
  if (on_ladder) {
    dynamic_obj.texture->change_state(1);
  } else {
    if (dynamic_obj.texture->curr_state == 1)
      dynamic_obj.texture->change_state(0);
  }
  dynamic_obj.draw(&screen);
}

void Player::move(MoveDirection dir, bool key_down) {
  // if lifted different key, stop moving
  if (!key_down) {
    if (dir == move_direction) {
      move_direction = DIR_NONE;
    }
    return;
  }
  if (on_ladder && (dir == DIR_LEFT || dir == DIR_RIGHT))
    return;

  move_direction = dir;
}

void Player::jump() {
  if (!dynamic_obj.coyote_on_ground)
    return;

  dynamic_obj.on_ground = false;
  dynamic_obj.coyote_on_ground = false;
  dynamic_obj.velocity.y = -JUMP_VELOCITY;
}
