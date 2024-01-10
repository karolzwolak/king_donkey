#pragma once

class Vector2 {

public:
  double x, y;

  Vector2(double _x, double _y);

  Vector2();

  Vector2 add(Vector2 v);
  Vector2 sub(Vector2 v);
  Vector2 mul(double v);
};