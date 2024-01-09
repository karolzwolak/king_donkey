#include "./SDL2-2.0.10/include/SDL.h"

#define ATLAS_PADDING 1
#define FRAME_PADDING 0

class AnimationFrames {
public:
  int atlas_x, atlas_y;
  int frame_count;

  AnimationFrames();
  AnimationFrames(int atlas_x, int atlas_y, int frame_count);
};

class AnimatedTexture {
  SDL_Rect rect;

public:
  static const int MAX_STATES = 10;
  /// each state is effectively an animation
  AnimationFrames *state_animations;
  ///  lookup to convert state into index of animation for easy access
  int *state_to_id_lookup;
  int state_count;

  int frame_width, frame_height;

  AnimatedTexture(int frame_width, int frame_height);
  ~AnimatedTexture();
  void add_state(int state_val, AnimationFrames frames);

  SDL_Rect *get_frame(int state_val, int frame);
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
