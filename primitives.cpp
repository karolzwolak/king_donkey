
class Vector2 {

public:
  float x, y;

  Vector2(float _x, float _y) {
    x = _x;
    y = _y;
  }

  Vector2() : Vector2(0, 0) {}

  Vector2 add(Vector2 v) { return Vector2(x + v.x, y + v.y); }
  Vector2 sub(Vector2 v) { return Vector2(x - v.x, y - v.y); }
  Vector2 mul(float v) { return Vector2(x * v, y * v); }
};
