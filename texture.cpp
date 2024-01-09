#include "texture.hpp"
#include <cassert>

AnimationFrames::AnimationFrames(int atlas_x, int atlas_y, int frame_count)
    : atlas_x(atlas_x), atlas_y(atlas_y), frame_count(frame_count){};

AnimationFrames::AnimationFrames() : atlas_x(0), atlas_y(0), frame_count(0){};

AnimatedTexture::AnimatedTexture(int frame_width, int frame_height)
    : frame_width(frame_width), frame_height(frame_height), state_count(0) {
  state_animations = new AnimationFrames[MAX_STATES];
  state_to_id_lookup = new int[MAX_STATES];
}

AnimatedTexture::~AnimatedTexture() {
  delete[] state_animations;
  delete[] state_to_id_lookup;
}

void AnimatedTexture::add_state(int state_val, AnimationFrames frames) {
  state_count++;
  assert(state_count < MAX_STATES);
  assert(state_val < MAX_STATES);

  state_animations[state_count] = frames;
  state_to_id_lookup[state_val] = state_count;
}
SDL_Rect *AnimatedTexture::get_frame(int state_val, int frame) {
  assert(state_val < MAX_STATES);

  AnimationFrames frames = state_animations[state_to_id_lookup[state_val]];
  rect.x = frames.atlas_x + frame * (frame_width + FRAME_PADDING);
  rect.y = frames.atlas_y;

  return &rect;
}

SimpleTexture::SimpleTexture(int atlas_x, int atlas_y, int width, int height)
    : atlas_x(atlas_x), atlas_y(atlas_y), width(width), height(height),
      rect({0, 0, width, height}) {}
