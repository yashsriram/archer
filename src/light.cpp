#include <simplecpp>
#include "utils.cpp"

class Light {
    vector<Line> rays;
    const Vector2d *epoch;
    Vector2d *head;
    double direction;
    double dx, dy;
public:
    explicit Light(const Vector2d epoch) : epoch(new Vector2d(epoch)),
                                           head(new Vector2d(epoch)),
                                           direction(0),
                                           dx(cosine(0)),
                                           dy(sine(0)) {}

    ~Light() {
        delete head;
        delete epoch;
    }

    Vector2d getHead() {
        return Vector2d(head);
    }

    void setDirection(double direction) {
        this->direction = direction;
        dx = cosine(direction);
        dy = sine(direction);
    }

    double getDirection() {
        return direction;
    }

    void newRay() {
        if (rays.empty()) {
            Line line(epoch->x, epoch->y, head->x, head->y);
            line.setColor(COLOR(255, 255, 255));
            rays.push_back(line);
        } else {
            Position penultimateHead = rays.rbegin()[0].getEnd();
            Line line(penultimateHead.x, penultimateHead.y, head->x, head->y);
            line.setColor(COLOR(255, 255, 255));
            rays.push_back(line);
        }
    }

    void moveHead(double factor) {
        head->x += factor * dx;
        head->y += factor * dy;
    }

    bool isHeadOnWall() {
        return head->x >= 1450 || head->x <= 50 || head->y >= 650 || head->y <= 50;
    }

    void reset() {
        rays.clear();
        head->set(epoch);
        direction = dx = dy = 0;
    }
};
