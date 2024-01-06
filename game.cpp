#include "objects.cpp"
#include "primitives.cpp"
#include "screen.cpp"
#include "world.cpp"

class Game {
  Screen screen;
  World world;

public:
  Game(int w, int h) : screen(w, h), world(w, h) {}

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
      world.player.move(LEFT);
      break;
    case SDLK_RIGHT:
      world.player.move(RIGHT);
      break;
    case SDLK_UP:
      world.player.move(UP);
      break;
    case SDLK_DOWN:
      world.player.move(DOWN);
      break;
      /* case SDLK_SPACE: */
      /*   world.player.jump(); */
    };
    return false;
  }

  bool process_input() {
    while (SDL_PollEvent(&screen.event)) {
      switch (screen.event.type) {
      case SDL_KEYUP:
        world.player.move(NONE);
        break;
      case SDL_KEYDOWN:
        return process_keydown(screen.event.key.keysym.sym);
      case SDL_QUIT:
        return true;
      };
    }
    return false;
  }

  void main_loop() {
    while (1) {
      bool quit = process_input();
      if (quit)
        break;
      update_and_render();
    };
  }
};
