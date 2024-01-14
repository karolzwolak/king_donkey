#include "texture.hpp"
#include "objects.hpp"
#include "world.hpp"
#include <cassert>

AnimationFrames::AnimationFrames(int atlas_x, int atlas_y, int frame_count,
                                 double time_per_frame, int repeat_from_frame,
                                 Orientation frame_orientation,
                                 bool update_when_stationary)
    : atlas_x(atlas_x), atlas_y(atlas_y), frame_count(frame_count),
      repeat_from_frame(repeat_from_frame), time_per_frame(time_per_frame),
      frame_orientation(frame_orientation),
      update_when_stationary(update_when_stationary){};

AnimationFrames::AnimationFrames() : atlas_x(0), atlas_y(0), frame_count(0){};

AnimatedTexture::AnimatedTexture(int frame_width, int frame_height)
    : frame_width(frame_width), frame_height(frame_height), animation_count(0),
      curr_animation(NULL), rect({0, 0, frame_width, frame_height}),
      curr_frame(0), timer(0), curr_orientation(OR_NONE), curr_state(0) {
  state_animations = new AnimationFrames[MAX_STATES];
  id_to_state = new int[MAX_STATES];
}

AnimatedTexture::~AnimatedTexture() {
  /* delete[] state_animations; */
  /* delete[] id_to_state; */
}

AnimationFrames *AnimatedTexture::find_animation(int state_val) {
  for (int i = 0; i < animation_count; i++) {
    if (id_to_state[i] == state_val) {
      return &state_animations[i];
    }
  }
  return nullptr;
}

void AnimatedTexture::reset_animation() {
  curr_frame = 0;
  timer = 0;
}

bool AnimatedTexture::needs_flipping() {
  return curr_orientation != OR_NONE &&
         curr_orientation != curr_animation->frame_orientation;
}

void AnimatedTexture::change_state(int state_val) {
  if (state_val == curr_state) {
    return;
  }

  AnimationFrames *animation = find_animation(state_val);
  assert(animation != nullptr);
  curr_state = state_val;

  if (animation != curr_animation) {
    reset_animation();
  }
  curr_animation = animation;
}

void AnimatedTexture::update(double delta, bool stationary) {
  if (stationary && !curr_animation->update_when_stationary) {
    return;
  }
  timer += delta;
  if (timer > curr_animation->time_per_frame) {
    timer = 0;
    curr_frame++;
    if (curr_frame >= curr_animation->frame_count) {
      curr_frame = curr_animation->repeat_from_frame;
    }
  }
}

void AnimatedTexture::add_animation(int state_val, AnimationFrames frames) {
  assert(animation_count < MAX_STATES);

  state_animations[animation_count] = frames;
  id_to_state[animation_count] = state_val;
  animation_count++;

  if (curr_animation == NULL) {
    curr_animation = &state_animations[0];
  }
}

SDL_Rect *AnimatedTexture::get_curr_frame() {
  assert(curr_animation != NULL);
  int frame_id = curr_frame;
  rect.x = curr_animation->atlas_x + frame_id * (frame_width + FRAME_PADDING);
  rect.y = curr_animation->atlas_y;

  return &rect;
}

void AnimatedTexture::draw(Screen *screen, int x, int y) {
  screen->draw_atlas_texture(get_curr_frame(), x, y, needs_flipping());
}

void AnimatedTexture::change_orientation(Orientation orientation) {
  curr_orientation = orientation;
}

SimpleTexture::SimpleTexture(int atlas_x, int atlas_y, int width, int height)
    : atlas_x(atlas_x), atlas_y(atlas_y), width(width), height(height),
      rect({atlas_x, atlas_y, width, height}) {}

TextureManager::TextureManager()
    : tile_texture(Tile::create_texture()),
      ladder_texture(Ladder::create_texture()) {}
