#include <cmath>

struct Vector2d {
    double x, y;

    Vector2d() : x(0), y(0) {}

    Vector2d(const double x, const double y) : x(x), y(y) {}

    Vector2d(const Vector2d *copy) : x(copy->x), y(copy->y) {}

    double length() { return sqrt(x * x + y * y); }

    Vector2d *setDiffOf(const Vector2d *a, const Vector2d *b) {
        this->set(a->x - b->x, a->y - b->y);
        return this;
    }

    Vector2d *set(const double x, const double y) {
        this->x = x;
        this->y = y;
        return this;
    }

    Vector2d *set(const Vector2d *copy) {
        return this->set(copy->x, copy->y);
    }
};
