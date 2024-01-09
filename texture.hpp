#pragma once

#include "./SDL2-2.0.10/include/SDL.h"

#define ATLAS_PADDING 1
#define FRAME_PADDING 0

class AnimationFrames {
public:
  constexpr static const double TIME_PER_FRAME = 0.1;

  int atlas_x, atlas_y;
  int frame_count;
  int repeat_from_frame;
  int curr_frame;
  double timer;

  AnimationFrames();
  AnimationFrames(int atlas_x, int atlas_y, int frame_count,
                  int repeat_from_frame);

  void update(double delta);
  void reset();
};

class AnimatedTexture {
  SDL_Rect rect;

public:
  static const int MAX_STATES = 10;
  /// each state is effectively an animation
  AnimationFrames *state_animations;
  ///  lookup to convert state into index of animation for easy access
  AnimationFrames *curr_animation;
  int *state_to_id_lookup;
  int animation_count;

  int frame_width, frame_height;

  AnimatedTexture(int frame_width, int frame_height);
  ~AnimatedTexture();
  void add_animation(int state_val, AnimationFrames frames);

  void change_state(int state_val);
  void update(double delta);

  SDL_Rect *get_curr_frame();
};

class SimpleTexture {

public:
  int atlas_x, atlas_y, width, height;
  SDL_Rect rect;

  SimpleTexture(int atlas_x, int atlas_y, int width, int height);
};

class TextureAtlas {
  SDL_Surface atlas;
};
