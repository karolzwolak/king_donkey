#pragma once

class Vector2 {

public:
  float x, y;

  Vector2(float _x, float _y);

  Vector2();

  Vector2 add(Vector2 v);
  Vector2 sub(Vector2 v);
  Vector2 mul(float v);
};
