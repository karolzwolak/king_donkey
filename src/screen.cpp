#include "screen.hpp"
#include "../SDL2-2.0.10/include/SDL_render.h"

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void Screen::draw_string(int x, int y, const char *text, SDL_Surface *charset) {
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
    SDL_BlitSurface(charset, &s, screen, &d);
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
  /* SDL_BlitSurface(atlas, sprite, screen, &dest); */
}
// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the top left corner of sprite on screen
void Screen::draw_sprite(SDL_Surface *sprite, int x, int y) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = sprite->w;
  dest.h = sprite->h;
  SDL_BlitSurface(sprite, NULL, screen, &dest);
};

// draw a single pixel
void Screen::draw_pixel(int x, int y, Uint32 color) {
  int bpp = screen->format->BytesPerPixel;
  Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;
  *(Uint32 *)p = color;
};
// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0)
// line
void Screen::draw_line(int x, int y, int l, int dx, int dy, Uint32 color) {
  for (int i = 0; i < l; i++) {
    draw_pixel(x, y, color);
    x += dx;
    y += dy;
  };
};
// draw a rectangle of size l by k
void Screen::draw_rect(int x, int y, int l, int k, Uint32 outlineColor,
                       Uint32 fillColor) {
  int i;
  draw_line(x, y, k, 0, 1, outlineColor);
  draw_line(x + l - 1, y, k, 0, 1, outlineColor);
  draw_line(x, y, l, 1, 0, outlineColor);
  draw_line(x, y + k - 1, l, 1, 0, outlineColor);
  for (i = y + 1; i < y + k - 1; i++)
    draw_line(x + 1, i, l - 2, 1, 0, fillColor);
}

void Screen::clear() {
  SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
  SDL_RenderClear(renderer);
  /* SDL_FillRect(screen.screen, NULL, czarny); */
}

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
    printf("fps: %f\n", fps);
  };
  frames++;

  limit_fps(fps_timer);

  return dt;
}

void Screen::render() {
  /* SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch); */
  /* SDL_RenderClear(renderer); */
  /* SDL_RenderCopy(renderer, scrtex, NULL, NULL); */
  SDL_RenderPresent(renderer);
}

Screen::Screen(int w, int h) {
  fps_timer = 0;
  fps = 0;
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

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  SDL_SetWindowTitle(window, WINDOW_TITLE);

  screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000,
                                0x0000FF00, 0x000000FF, 0xFF000000);

  scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                             SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                             SCREEN_HEIGHT);

  SDL_Surface *surface = SDL_LoadBMP("assets/atlas.bmp");
  atlas = SDL_CreateTextureFromSurface(renderer, surface);
  if (atlas == NULL) {
    throw "Cannot load file resources/atlas.png\n";
    printf("Cannot load file resources/atlas.png\n");
  }
  SDL_FreeSurface(surface);

  t1 = SDL_GetTicks();
  t2 = t1;
}

Screen::~Screen() {
  SDL_FreeSurface(screen);
  SDL_DestroyTexture(scrtex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(atlas);

  SDL_Quit();
}
