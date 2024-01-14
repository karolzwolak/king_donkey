#include "objects.cpp"
#include "primitives.cpp"
#include "screen.cpp"
#include "screen.hpp"
#include "texture.cpp"
#include "world.cpp"

class Game {
  Screen screen;
  World world;
  TextureManager textures;

public:
  Game(int w, int h)
      : screen(w, h), textures(),
        world(LOGICAL_SCREEN_WIDTH, LOGICAL_SCREEN_HEIGHT, textures) {}

  void clear() { screen.clear(); }
  void update(double dt) { world.update(dt); }
  void draw_world() { world.draw(screen); }

  void update_and_render() {
    clear();
    double dt = screen.tick();
    update(dt);
    draw_world();
    screen.render();
  }

  bool process_keydown(SDL_Keycode key) {
    switch (key) {
    case SDLK_ESCAPE:
      return true;
    case SDLK_LEFT:
      world.player.move(DIR_LEFT, true);
      break;
    case SDLK_RIGHT:
      world.player.move(DIR_RIGHT, true);
      break;
    case SDLK_UP:
      world.player.move(DIR_UP, true);
      break;
    case SDLK_DOWN:
      world.player.move(DIR_DOWN, true);
      break;
    case SDLK_SPACE:
      world.player.jump();
      break;
    case SDLK_n:
      world.reset_level();
      break;
    };
    return false;
  }

  bool process_keyup(SDL_Keycode key) {
    switch (key) {
    case SDLK_ESCAPE:
      return true;
    case SDLK_LEFT:
      world.player.move(DIR_LEFT, false);
      break;
    case SDLK_RIGHT:
      world.player.move(DIR_RIGHT, false);
      break;
    case SDLK_UP:
      world.player.move(DIR_UP, false);
      break;
    case SDLK_DOWN:
      world.player.move(DIR_DOWN, false);
      break;
    };
    return false;
  }

  bool process_input() {
    while (SDL_PollEvent(&screen.event)) {
      switch (screen.event.type) {
      case SDL_KEYUP:
        return process_keyup(screen.event.key.keysym.sym);
      case SDL_KEYDOWN:
        return process_keydown(screen.event.key.keysym.sym);
      case SDL_QUIT:
        return true;
      };
    }
    return false;
  }

  void main_loop() {
    bool quit = false;
    while (!quit) {
      quit = process_input();
      update_and_render();
    };
  }
};
