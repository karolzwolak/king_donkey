#include "primitives.hpp"

Vector2::Vector2(double _x, double _y) : x(_x), y(_y) {}
Vector2::Vector2() : Vector2(0, 0) {}

Vector2 Vector2::add(Vector2 v) { return Vector2(x + v.x, y + v.y); }
Vector2 Vector2::sub(Vector2 v) { return Vector2(x - v.x, y - v.y); }
Vector2 Vector2::mul(double v) { return Vector2(x * v, y * v); }
