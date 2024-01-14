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

bool RectObject::collides_with(RectObject *obj) {
  return left() < obj->right() && right() > obj->left() &&
         top() < obj->bottom() && bottom() > obj->top();
}

Ladder::Ladder(Vector2 pos, int parts, SimpleTexture *texture)
    : static_obj(pos, LADDER_WIDTH, parts * LADDER_HEIGHT, texture),
      parts(parts) {}

Ladder::Ladder() : Ladder(Vector2(0, 0), 0, NULL) {}

SimpleTexture Ladder::create_texture() {
  SimpleTexture texture(50, 35, 8, 8);
  return texture;
}

RectObject *Ladder::get_rect() { return &static_obj.obj; }

StaticObject::StaticObject(Vector2 pos, int w, int h, SimpleTexture *texture)
    : obj(pos, w, h), texture(texture) {}

void StaticObject::draw(Screen *screen) {
  screen->draw_atlas_texture(&texture->rect, obj.pos.x, obj.pos.y);
}

void Ladder::draw(Screen &screen) {
  Vector2 pos = static_obj.obj.pos;

  for (int i = 0; i < static_obj.obj.height; i += LADDER_HEIGHT) {
    static_obj.obj.pos.y = pos.y + i;
    static_obj.draw(&screen);
  }
  static_obj.obj.pos = pos;
}

Tile::Tile(Vector2 pos, SimpleTexture *texture)
    : static_obj(pos, TILE_WIDTH, TILE_HEIGHT, texture) {}

Tile::Tile() : Tile(Vector2(0, 0), NULL) {}

SimpleTexture Tile::create_texture() { return SimpleTexture(60, 35, 16, 8); }

RectObject *Tile::get_rect() { return &static_obj.obj; }

void Tile::draw(Screen &screen) { static_obj.draw(&screen); }

DynamicObject::DynamicObject(Vector2 pos, int w, int h,
                             double horizontal_move_vel,
                             AnimatedTexture texture)
    : rect_obj(pos, w, h), velocity(0, 0),
      horizontal_move_vel(horizontal_move_vel), acceleration(0, GRAVITY),
      on_ground(false), orientation(OR_NONE), texture(texture) {}

void DynamicObject::limit_velocity() {
  if (abs(velocity.x) > MAX_VELOCITY)
    velocity.x = sign(velocity.x) * MAX_VELOCITY;
  if (abs(velocity.y) > MAX_VELOCITY)
    velocity.y = sign(velocity.y) * MAX_VELOCITY;
}

void DynamicObject::set_velocity(Vector2 v) { velocity = v; }

void DynamicObject::set_acceleration_x(float x) { acceleration.x = x; }

bool DynamicObject::check_tile_collisions_x(World *world) {
  bool out_of_bounds = check_out_of_bounds_x(world);
  for (int i = 0; i < world->tile_count; i++) {
    RectObject *tile_rect = world->tiles[i].get_rect();
    if (rect_obj.collides_with(tile_rect)) {

      double elevation_dist = rect_obj.bottom() - tile_rect->top();
      if (velocity.y == 0 && elevation_dist > 0 &&
          elevation_dist <= CLIMB_THRESHOLD) {
        rect_obj.pos.y -= elevation_dist;
        return false;
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
  return out_of_bounds;
}

bool DynamicObject::check_tile_collisions_y(World *world, bool check_bounds) {
  on_ground = false;

  bool out_of_bounds = false;
  if (check_bounds) {
    out_of_bounds = check_out_of_bounds_y(world);
  }

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

  return out_of_bounds;
}

bool DynamicObject::check_out_of_bounds_x(World *world) {
  if (rect_obj.left() < 0) {

    rect_obj.pos.x = 0;
    return true;
  } else if (rect_obj.right() > world->width) {

    rect_obj.pos.x = world->width - rect_obj.width;
    return true;
  }

  return false;
}

bool DynamicObject::check_out_of_bounds_y(World *world) {
  if (rect_obj.top() < 0) {
    rect_obj.pos.y = 0;
    return true;
  }
  if (rect_obj.bottom() > world->height) {

    on_ground = true;
    coyote_on_ground = true;
    velocity.y = 0;

    rect_obj.pos.y = world->height - rect_obj.height;
    return true;
  }
  return false;
}

void DynamicObject::horizontal_movement(MoveDirection dir, double dt) {
  velocity.x = 0;

  if (dir == DIR_LEFT) {
    velocity.x = -horizontal_move_vel;
    orientation = OR_LEFT;
  } else if (dir == DIR_RIGHT) {
    velocity.x = horizontal_move_vel;
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

void DynamicObject::reset(Vector2 pos) {
  rect_obj.pos = pos;
  velocity = Vector2(0, 0);
  acceleration = Vector2(0, GRAVITY);
}

void DynamicObject::update_texture(double dt, int state) {
  texture.update(dt, velocity.x == 0 && velocity.y == 0);
  texture.change_orientation(orientation);

  if (state >= 0)
    texture.change_state(state);
}

void DynamicObject::update(MoveDirection dir, World *world, double dt) {
  update_texture(dt);

  horizontal_movement(dir, dt);
  check_tile_collisions_x(world);

  vertical_movement(dir, dt);
  check_tile_collisions_y(world);
}

void DynamicObject::draw(Screen *screen) {
  texture.draw(screen, rect_obj.pos.x, rect_obj.pos.y);
}

Player::Player(Vector2 pos)
    : dynamic_obj(pos, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_MOVE_VEL,
                  Player::create_texture()),
      on_ladder(false), move_direction(DIR_NONE) {}

AnimatedTexture Player::create_texture() {
  AnimatedTexture texture(16, 16);

  AnimationFrames idle_frames = AnimationFrames(0, 17, 1, 0, 0, OR_RIGHT);
  AnimationFrames run_frames = AnimationFrames(0, 17, 3, 0.15, 0, OR_RIGHT);
  AnimationFrames climb_frames =
      AnimationFrames(49, 17, 2, 0.15, 0, OR_NONE, false);

  AnimationFrames jump_frames = AnimationFrames(0, 0, 6, 0.1, 5);
  AnimationFrames fall_frames = AnimationFrames(80, 0, 1, 0, 0);

  texture.add_animation(IDLE, idle_frames);
  texture.add_animation(WALKING, run_frames);
  texture.add_animation(CLIMBING, climb_frames);
  texture.add_animation(JUMPING, jump_frames);
  texture.add_animation(FALLING, fall_frames);

  return texture;
}

void Player::player_vertical_movement(double dt) {
  if (on_ladder) {
    state = CLIMBING;
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

  else if (dynamic_obj.coyote_on_ground) {
    if (dynamic_obj.velocity.x == 0) {
      state = IDLE;
    } else {
      state = WALKING;
    }
  } else if (dynamic_obj.velocity.y > 0) {
    state = FALLING;
  }

  dynamic_obj.vertical_movement(move_direction, dt);
}

Barrel::Barrel(Vector2 pos)
    : dynamic_obj(pos, BARREL_WIDTH, BARREL_HEIGHT, BARREL_MOVE_VEL,
                  Barrel::create_texture()),
      move_direction(DIR_NONE), fallen_off(false) {
  dynamic_obj.coyote_on_ground = true;
  dynamic_obj.on_ground = true;
  freeze();
}

Barrel::Barrel() : Barrel(Vector2(0, 0)){};

void Barrel::start_moving(MoveDirection dir) {
  fallen_off = false;
  dynamic_obj.acceleration.y = GRAVITY;
  move_direction = dir;
}

void Barrel::freeze() {
  dynamic_obj.acceleration.y = 0;
  move_direction = DIR_NONE;
}

AnimatedTexture Barrel::create_texture() {
  AnimatedTexture texture(BARREL_WIDTH, BARREL_HEIGHT);
  AnimationFrames walk_frames = AnimationFrames(0, 34, 4, 0.1, 0, OR_NONE);
  texture.add_animation(WALKING, walk_frames);
  return texture;
}

RectObject &Barrel::get_rect() { return dynamic_obj.rect_obj; }

void Barrel::update(World *world, double dt) {
  dynamic_obj.update_texture(dt);

  bool was_coyote = dynamic_obj.coyote_on_ground;

  dynamic_obj.horizontal_movement(move_direction, dt);
  bool hit_wall = dynamic_obj.check_tile_collisions_x(world);

  dynamic_obj.vertical_movement(move_direction, dt);
  bool hit_ground = dynamic_obj.check_tile_collisions_y(world, false);

  if (dynamic_obj.rect_obj.bottom() > world->height) {
    fallen_off = true;
    move_direction = DIR_NONE;
    return;
  }

  // if barrel hits wall, or falls off ledge, reverse direction
  if (hit_wall || (hit_ground && !was_coyote))
    move_direction = opposite_direction(move_direction);
}

void Barrel::draw(Screen &screen) { dynamic_obj.draw(&screen); }

RectObject &Player::get_rect() { return dynamic_obj.rect_obj; }

bool Player::still_on_ladder(World *world) {
  return (move_direction == DIR_DOWN &&
          ladder->get_rect()->bottom() >= get_rect().bottom()) ||
         ladder->get_rect()->collides_with(&get_rect());
}

void Player::update(World *world, double dt) {
  dynamic_obj.update_texture(dt);

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

  if (on_ladder && !still_on_ladder(world)) {
    dynamic_obj.check_tile_collisions_y(world);
    get_off_ladder();
  }

  if (!on_ladder) {
    dynamic_obj.check_tile_collisions_y(world);
  }
}

void Player::get_on_ladder(Ladder *new_ladder) {
  if (new_ladder == NULL)
    return;
  ladder = new_ladder;

  on_ladder = true;
  state = CLIMBING;

  get_rect().pos.x = ladder->get_rect()->center_x() - get_rect().width / 2.;
  dynamic_obj.velocity.y = 0;
  dynamic_obj.acceleration.y = 0;
  move_direction = DIR_NONE;
}

void Player::get_off_ladder() {
  state = FALLING;
  ladder = NULL;

  on_ladder = false;
  dynamic_obj.acceleration.y = GRAVITY;
  if (move_direction == DIR_UP)
    dynamic_obj.velocity.y = -JUMP_VELOCITY / 4;
  else
    dynamic_obj.velocity.y = 0;
  move_direction = DIR_NONE;
}

void Player::draw(Screen &screen) {
  dynamic_obj.texture.change_state(state);
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

  state = JUMPING;

  dynamic_obj.on_ground = false;
  dynamic_obj.coyote_on_ground = false;
  dynamic_obj.velocity.y = -JUMP_VELOCITY;
}
