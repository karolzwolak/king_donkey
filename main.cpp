#define _USE_MATH_DEFINES
#include "game.cpp"

// main
#ifdef __cplusplus
extern "C"
#endif
    int
    main(int argc, char **argv) {
  Game game(SCREEN_WIDTH, SCREEN_HEIGHT);

  game.main_loop();

  /* return example(); */
  return 0;
};
