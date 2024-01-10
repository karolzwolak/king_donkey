#include "texture.hpp"
#include <cassert>

AnimationFrames::AnimationFrames(int atlas_x, int atlas_y, int frame_count,
                                 double time_per_frame, int repeat_from_frame,
                                 Orientation frame_orientation)
    : atlas_x(atlas_x), atlas_y(atlas_y), frame_count(frame_count),
      repeat_from_frame(repeat_from_frame), curr_frame(0), timer(0),
      time_per_frame(time_per_frame), frame_orientation(frame_orientation),
      curr_orientation(OR_NONE){};

AnimationFrames::AnimationFrames() : atlas_x(0), atlas_y(0), frame_count(0){};

void AnimationFrames::update(double delta) {
  timer += delta;
  if (timer > time_per_frame) {
    timer = 0;
    curr_frame++;
    if (curr_frame >= frame_count) {
      curr_frame = repeat_from_frame;
    }
  }
}

void AnimationFrames::reset() {
  curr_frame = 0;
  timer = 0;
}

bool AnimationFrames::needs_flipping() {
  return curr_orientation != OR_NONE && curr_orientation != frame_orientation;
}

AnimatedTexture::AnimatedTexture(int frame_width, int frame_height)
    : frame_width(frame_width), frame_height(frame_height), animation_count(0),
      curr_animation(NULL), rect({0, 0, frame_width, frame_height}) {
  state_animations = new AnimationFrames[MAX_STATES];
  state_to_id_lookup = new int[MAX_STATES];
}

AnimatedTexture::~AnimatedTexture() {
  delete[] state_animations;
  delete[] state_to_id_lookup;
}

void AnimatedTexture::change_state(int state_val) {
  assert(state_val < MAX_STATES);
  int state_id = state_to_id_lookup[state_val];
  if (&state_animations[state_id] != curr_animation) {
    curr_animation->reset();
  }
  curr_animation = &state_animations[state_id];
}

void AnimatedTexture::update(double delta) { curr_animation->update(delta); }

void AnimatedTexture::add_animation(int state_val, AnimationFrames frames) {
  assert(animation_count < MAX_STATES);
  assert(state_val < MAX_STATES);

  state_animations[animation_count] = frames;
  state_to_id_lookup[state_val] = animation_count;
  animation_count++;

  if (curr_animation == NULL) {
    curr_animation = &state_animations[0];
  }
}

bool AnimatedTexture::needs_flipping() {
  return curr_animation->needs_flipping();
}

SDL_Rect *AnimatedTexture::get_curr_frame() {
  assert(curr_animation != NULL);
  int frame_id = curr_animation->curr_frame;
  rect.x = curr_animation->atlas_x + frame_id * (frame_width + FRAME_PADDING);
  rect.y = curr_animation->atlas_y;

  return &rect;
}

void AnimatedTexture::draw(Screen *screen, int x, int y) {
  screen->draw_atlas_texture(get_curr_frame(), x, y, needs_flipping());
}

void AnimatedTexture::change_orientation(Orientation orientation) {
  curr_animation->curr_orientation = orientation;
}

SimpleTexture::SimpleTexture(int atlas_x, int atlas_y, int width, int height)
    : atlas_x(atlas_x), atlas_y(atlas_y), width(width), height(height),
      rect({atlas_x, atlas_y, width, height}) {}
