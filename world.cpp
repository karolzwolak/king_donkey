#include "world.hpp"

World::World(int w, int h)
    : width(w), height(h), player(Player(Vector2(10, 10), 16, 32)) {}

void World::update(double dt) { player.update(dt); }
void World::draw(Screen &screen) { player.draw(screen); }
