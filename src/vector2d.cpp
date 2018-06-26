#include <cmath>

struct Vector2d {
    double x, y;

    Vector2d() : x(0), y(0) {}

    Vector2d(double x, double y) : x(x), y(y) {}

    double length() {
        return sqrt(x * x + y * y);
    }

    Vector2d* setDiffOf(const Vector2d *a, const Vector2d *b) {
        this->set(a->x - b->x, a->y - b->y);
        return this;
    }

    Vector2d* set(double x, double y) {
        this->x = x;
        this->y = y;
        return this;
    }
};
