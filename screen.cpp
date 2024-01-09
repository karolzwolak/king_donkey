#include "screen.hpp"
#include "SDL2-2.0.10/include/SDL_render.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define WINDOW_TITLE "King Donkey game"

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

void Screen::draw_atlas_texture(SDL_Rect *sprite, int x, int y) {
  SDL_Rect dest;
  dest.x = x;
  dest.y = y;
  dest.w = sprite->w;
  dest.h = sprite->h;
  /* SDL_RenderCopy(renderer, atlas, sprite, &dest); */
  SDL_RenderCopy(renderer, atlas, sprite, &dest);
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
  /* SDL_FillRect(screen.screen, NULL, czarny); */
}

void Screen::sleep(double ms) {
  if (ms <= 0)
    return;
  SDL_Delay(ms);
}

void Screen::limit_fps() {
  double expected_time = 1000.0 / target_fps;
  double dt_ms = dt * 1000.0;
  double delay = expected_time - dt_ms;
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

  limit_fps();

  return dt;
}

void Screen::render() {
  SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, scrtex, NULL, NULL);
  SDL_RenderPresent(renderer);
}

Screen::Screen(int w, int h) {
  width = w;
  height = h;
  fps_timer = 0;
  fps = 0;
  target_fps = TARGET_FPS;

  t1 = SDL_GetTicks();

  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("SDL_Init error: %s\n", SDL_GetError());
    throw "SDL_Init failed";
  }

  // fullscreen mode
  //	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
  //	                                 &window, &renderer);
  int rc = SDL_CreateWindowAndRenderer(w, h, 0, &window, &renderer);
  if (rc != 0) {
    SDL_Quit();
    printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
    throw "SDL_CreateWindowAndRenderer failed";
  };

  SDL_Surface *surface = SDL_LoadBMP("resources/atlas.bmp");
  atlas = SDL_CreateTextureFromSurface(renderer, surface);
  if (atlas == NULL) {
    throw "Cannot load file resources/atlas.png\n";
    printf("Cannot load file resources/atlas.png\n");
  }
  SDL_FreeSurface(surface);

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

  SDL_SetWindowTitle(window, WINDOW_TITLE);

  screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000,
                                0x0000FF00, 0x000000FF, 0xFF000000);

  scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                             SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH,
                             SCREEN_HEIGHT);
}

Screen::~Screen() {
  SDL_FreeSurface(screen);
  SDL_DestroyTexture(scrtex);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_DestroyTexture(atlas);

  SDL_Quit();
}

int example() {
  int t1, t2, quit, frames, rc;
  double delta, worldTime, fpsTimer, fps, distance, etiSpeed;
  SDL_Event event;
  SDL_Surface *charset;
  SDL_Surface *eti;

  Screen screen(SCREEN_WIDTH, SCREEN_HEIGHT);

  // wy³¹czenie widocznoci kursora myszy
  SDL_ShowCursor(SDL_DISABLE);

  // wczytanie obrazka cs8x8.bmp
  charset = SDL_LoadBMP("resources/cs8x8.bmp");
  if (charset == NULL) {
    printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
    return 1;
  };
  SDL_SetColorKey(charset, true, 0x000000);

  eti = SDL_LoadBMP("resources/eti.bmp");
  if (eti == NULL) {
    printf("SDL_LoadBMP(eti.bmp) error: %s\n", SDL_GetError());
    SDL_FreeSurface(charset);
    return 1;
  };

  char text[128];
  int czarny = SDL_MapRGB(screen.screen->format, 0x00, 0x00, 0x00);
  int zielony = SDL_MapRGB(screen.screen->format, 0x00, 0xFF, 0x00);
  int czerwony = SDL_MapRGB(screen.screen->format, 0xFF, 0x00, 0x00);
  int niebieski = SDL_MapRGB(screen.screen->format, 0x11, 0x11, 0xCC);

  t1 = SDL_GetTicks();

  frames = 0;
  fpsTimer = 0;
  fps = 0;
  quit = 0;
  worldTime = 0;
  distance = 0;
  etiSpeed = 1;

  while (!quit) {
    t2 = SDL_GetTicks();

    // here t2-t1 is the time in milliseconds since
    // the last screen was drawn
    // delta is the same time in seconds
    delta = (t2 - t1) * 0.001;
    t1 = t2;

    worldTime += delta;

    distance += etiSpeed * delta;

    SDL_FillRect(screen.screen, NULL, czarny);

    screen.draw_sprite(eti,
                       SCREEN_WIDTH / 2 + sin(distance) * SCREEN_HEIGHT / 3,
                       SCREEN_HEIGHT / 2 + cos(distance) * SCREEN_HEIGHT / 3);

    fpsTimer += delta;
    if (fpsTimer > 0.5) {
      fps = frames * 2;
      frames = 0;
      fpsTimer -= 0.5;
    };

    // tekst informacyjny / info text
    screen.draw_rect(4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);
    //            "template for the second project, elapsed time = %.1lf s %.0lf
    //            frames / s"
    sprintf(
        text,
        "Szablon drugiego zadania, czas trwania = %.1lf s  %.0lf klatek / s",
        worldTime, fps);
    screen.draw_string(screen.width / 2 - strlen(text) * 8 / 2, 10, text,
                       charset);
    //	      "Esc - exit, \030 - faster, \031 - slower"
    sprintf(text, "Esc - wyjscie, \030 - przyspieszenie, \031 - zwolnienie");
    screen.draw_string(screen.width / 2 - strlen(text) * 8 / 2, 26, text,
                       charset);

    SDL_UpdateTexture(screen.scrtex, NULL, screen.screen->pixels,
                      screen.screen->pitch);
    //		SDL_RenderClear(renderer);
    SDL_RenderCopy(screen.renderer, screen.scrtex, NULL, NULL);
    SDL_RenderPresent(screen.renderer);

    // obs³uga zdarzeñ (o ile jakie zasz³y) / handling of events (if there
    // were any)
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
          quit = 1;
        else if (event.key.keysym.sym == SDLK_UP)
          etiSpeed = 2.0;
        else if (event.key.keysym.sym == SDLK_DOWN)
          etiSpeed = 0.3;
        break;
      case SDL_KEYUP:
        etiSpeed = 1.0;
        break;
      case SDL_QUIT:
        quit = 1;
        break;
      };
    };
    frames++;
  };

  // zwolnienie powierzchni / freeing all surfaces
  SDL_FreeSurface(charset);

  return 0;
}
