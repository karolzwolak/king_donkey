#include "screen.hpp"
#include "../SDL2-2.0.10/include/SDL_render.h"

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void Screen::draw_string(int x, int y, const char *text) {
  int px, py, c;
  SDL_Rect s, d;
  s.w = 8;
  s.h = 8;
  d.w = 8;
  d.h = 8;
  while (*text) {
    c = *text & 255;
    px = (c % 16) * 8;
    py = (c / 16) * 8;
    s.x = px;
    s.y = py;
    d.x = x;
    d.y = y;
    SDL_RenderCopy(renderer, charset, &s, &d);
    x += 8;
    text++;
  };
};

void Screen::draw_atlas_texture(SDL_Rect *sprite, double _x, double _y,
                                bool flip) {
  int x = round(_x);
  int y = round(_y);

  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = sprite->w;
  dest.h = sprite->h;
  if (flip) {
    SDL_RenderCopyEx(renderer, atlas, sprite, &dest, 0, NULL,
                     SDL_FLIP_HORIZONTAL);
    return;
  }
  SDL_RenderCopy(renderer, atlas, sprite, &dest);
}

void Screen::clear() { SDL_RenderClear(renderer); }

void Screen::sleep(double ms) {
  if (ms <= 0)
    return;
  SDL_Delay(ms);
}

void Screen::limit_fps(double fps_timer) {
  double expected_time = 1000.0 / target_fps;
  double delay = expected_time - fps_timer;
  sleep(delay);
}

double Screen::tick() {
  t2 = SDL_GetTicks();
  double dt_ms = (t2 - t1);
  t1 = t2;
  dt = dt_ms * 0.001;

  // count frames per second
  fps_timer += dt;
  if (fps_timer > 0.5) {
    fps = frames * 2;
    frames = 0;
    fps_timer -= 0.5;
  };
  frames++;

  limit_fps(fps_timer);

  return dt;
}

void Screen::render() { SDL_RenderPresent(renderer); }

Screen::Screen(int w, int h) {
  fps_timer = 0;
  fps = 0;
  dt = 0;
  frames = 0;
  target_fps = TARGET_FPS;

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("SDL_Init error: %s\n", SDL_GetError());
    throw "SDL_Init failed";
  }

  int rc;
  if (w * h == 0) {
    rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
                                     &window, &renderer);
  } else {
    rc = SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
  }

  if (rc != 0) {
    SDL_Quit();
    printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
    throw "SDL_CreateWindowAndRenderer failed";
  }
  SDL_GetWindowSize(window, &width, &height);

  SDL_RenderSetLogicalSize(renderer, LOGICAL_SCREEN_WIDTH,
                           LOGICAL_SCREEN_HEIGHT);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  SDL_SetWindowTitle(window, WINDOW_TITLE);

  SDL_Surface *surface = SDL_LoadBMP("assets/atlas.bmp");
  atlas = SDL_CreateTextureFromSurface(renderer, surface);
  if (atlas == NULL) {
    printf("Cannot load file resources/atlas.png\n");
    throw "Cannot load file resources/atlas.png\n";
  }
  SDL_FreeSurface(surface);

  surface = SDL_LoadBMP("assets/cs8x8.bmp");
  charset = SDL_CreateTextureFromSurface(renderer, surface);
  if (charset == NULL) {
    printf("Cannot load file resources/charset.bmp\n");
    throw "Cannot load file resources/charset.bmp\n";
  }
  SDL_FreeSurface(surface);

  t1 = SDL_GetTicks();
  t2 = t1;
}

Screen::~Screen() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(atlas);

  SDL_Quit();
}
