#pragma once

extern "C" {
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../SDL2-2.0.10/include/SDL.h"
#include "../SDL2-2.0.10/include/SDL_render.h"
}

#define LOGICAL_SCREEN_WIDTH 640
#define LOGICAL_SCREEN_HEIGHT 360

/* #define TARGET_FPS 120 */
#define TARGET_FPS 0
#define WINDOW_TITLE "King Donkey game"

class Screen {
  int t1, t2, frames;
  int target_fps;
  double dt, fps_timer, fps;

public:
  int width, height;

  SDL_Texture *atlas;
  SDL_Window *window;
  SDL_Renderer *renderer;

  SDL_Texture *charset;
  SDL_Event event;

  // draw a text txt on surface screen, starting from the point (x, y)
  // charset is a 128x128 bitmap containing character images
  void draw_string(int x, int y, const char *text);

  void draw_atlas_texture(SDL_Rect *sprite, double x, double y,
                          bool flip = false);

  void clear();
  void sleep(double ms);
  void limit_fps(double fps_timer);
  double tick();
  void render();

  Screen(int w, int h);

  ~Screen();
};

int example();
