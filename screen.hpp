#pragma once

extern "C" {
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "./SDL2-2.0.10/include/SDL.h"
/* #include "./SDL2-2.0.10/include/SDL_main.h" */
}

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define TARGET_FPS 120
#define WINDOW_TITLE "King Donkey game"

class Screen {
  int t1, t2, frames;
  int target_fps;
  double dt, fps_timer, fps;

public:
  int width, height;

  SDL_Surface *screen;
  SDL_Texture *scrtex;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Event event;

  // draw a text txt on surface screen, starting from the point (x, y)
  // charset is a 128x128 bitmap containing character images
  void draw_string(int x, int y, const char *text, SDL_Surface *charset);

  // draw a surface sprite on a surface screen in point (x, y)
  // (x, y) is the left top corner
  void draw_sprite(SDL_Surface *sprite, int x, int y);

  // draw a single pixel
  void draw_pixel(int x, int y, Uint32 color);
  // draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0)
  // line
  void draw_line(int x, int y, int l, int dx, int dy, Uint32 color);
  // draw a rectangle of size l by k
  void draw_rect(int x, int y, int l, int k, Uint32 outlineColor,
                 Uint32 fillColor);

  void clear();
  void sleep(double ms);
  void limit_fps();
  double tick();
  void render();

  Screen(int w, int h);

  ~Screen();
};

int example();
