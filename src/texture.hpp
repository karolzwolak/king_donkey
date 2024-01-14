#pragma once

#include "screen.hpp"

#define ATLAS_PADDING 1
#define FRAME_PADDING 0
#define DEFAULT_ORIENTATION OR_LEFT

enum Orientation { OR_LEFT, OR_RIGHT, OR_NONE };

class AnimationFrames {
public:
  int atlas_x, atlas_y;
  int frame_count;
  int repeat_from_frame;

  double time_per_frame;
  Orientation frame_orientation;
  bool update_when_stationary;

  AnimationFrames();
  AnimationFrames(int atlas_x, int atlas_y, int frame_count,
                  double time_per_frame, int repeat_from_frame,
                  Orientation frame_orientation = DEFAULT_ORIENTATION,
                  bool update_when_stationary = true);
};

class AnimatedTexture {
  SDL_Rect rect;

public:
  static const int MAX_STATES = 10;
  /// each state is effectively an animation
  AnimationFrames *state_animations;
  ///  lookup to convert state into index of animation for easy access
  AnimationFrames *curr_animation;
  int *id_to_state;
  int curr_state;
  int animation_count;

  double timer;
  int curr_frame;
  Orientation curr_orientation;

  int frame_width, frame_height;

  AnimatedTexture(int frame_width, int frame_height);
  AnimatedTexture();

  void add_animation(int state_val, AnimationFrames frames);
  void delete_animations();

  void change_state(int state_val);
  void update(double delta, bool stationary);
  void change_orientation(Orientation orientation);

  AnimationFrames *find_animation(int state_val);

  void reset_animation();

  SDL_Rect *get_curr_frame();
  void draw(Screen *screen, int x, int y);
  bool needs_flipping();
};

class SimpleTexture {

public:
  int atlas_x, atlas_y, width, height;
  SDL_Rect rect;

  SimpleTexture(int atlas_x, int atlas_y, int width, int height);
  void draw(Screen *screen, int x, int y);
};

class StaticTextureManager {
public:
  SimpleTexture tile_texture;
  SimpleTexture ladder_texture;

  StaticTextureManager();
};
