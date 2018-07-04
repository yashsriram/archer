#include<simplecpp>
#include <sstream>
#include "utils.cpp"

class Light {
    Color LIGHT_COLOR = COLOR(255, 255, 255);

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
            line.setColor(LIGHT_COLOR);
            rays.push_back(line);
        } else {
            Position penultimateHead = rays.rbegin()[0].getEnd();
            Line line(penultimateHead.x, penultimateHead.y, head->x, head->y);
            line.setColor(LIGHT_COLOR);
            rays.push_back(line);
        }
    }

    void moveHead(double factor) {
        head->x += factor * dx;
        head->y += factor * dy;
    }

    void reset() {
        rays.clear();
        head->set(epoch);
        direction = dx = dy = 0;
    }
};

class Mirrors {
    constexpr static double RADIUS_OF_CIRCLE = 75;
    constexpr static double SIDE_LENGTH_OF_SQUARE = 80;
    constexpr static double MIRROR_APPROACH_MARGIN = 0.5;

    Color MIRROR_COLOR = COLOR(160, 160, 160);
    Color INSTRUCTION_COLOR = COLOR(255, 255, 255);

    vector<Line> lineMirrors;
    vector<Circle> circleMirrors;

    void placeLineMirrors(int noLineMirrors) {
        double x1, y1, x2, y2;
        Vector2d click;
        Text instruction;
        instruction.setColor(INSTRUCTION_COLOR);
        for (int i = 0; i < noLineMirrors; i++) {
            ostringstream oss;
            oss << "Click at two points to place a line mirror, " << noLineMirrors - i << " left";
            instruction.reset(200, 25, oss.str());
            registerClick(&click);
            x1 = click.x;
            y1 = click.y;

            registerClick(&click);
            x2 = click.x;
            y2 = click.y;

            Line newLineMirror = Line(x1, y1, x2, y2);
            newLineMirror.setColor(MIRROR_COLOR);
            lineMirrors.push_back(newLineMirror);
        }
    }

    void placeCircleMirrors(int noCircleMirrors) {
        Vector2d click;
        Text instruction;
        instruction.setColor(INSTRUCTION_COLOR);
        for (int i = 0; i < noCircleMirrors; i++) {
            ostringstream oss;
            oss << "Click at a point to place a circle mirror, " << noCircleMirrors - i << " left";
            instruction.reset(200, 25, oss.str());
            registerClick(&click);
            Circle newCircleMirror = Circle(click.x, click.y, RADIUS_OF_CIRCLE);
            newCircleMirror.setColor(MIRROR_COLOR).setFill();
            circleMirrors.push_back(newCircleMirror);
        }
    }

    /**
     * Square mirror is nothing but set of 4 Line mirrors
     * */
    void placeSquareMirrors(int noSquareMirrors) {
        double x1, y1;
        Vector2d click;
        Text instruction;
        instruction.setColor(INSTRUCTION_COLOR);
        for (int i = 0; i < noSquareMirrors; i++) {
            ostringstream oss;
            oss << "Click at a point to place a square mirror, " << noSquareMirrors - i << " left";
            instruction.reset(200, 25, oss.str());

            registerClick(&click);
            x1 = click.x;
            y1 = click.y;

            Line a = Line(
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2,
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2);
            a.setColor(MIRROR_COLOR);
            lineMirrors.push_back(a);

            Line b = Line(
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2,
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2);
            b.setColor(MIRROR_COLOR);
            lineMirrors.push_back(b);

            Line c = Line(
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2,
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2);
            c.setColor(MIRROR_COLOR);
            lineMirrors.push_back(c);

            Line d = Line(
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2,
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2);
            d.setColor(MIRROR_COLOR);
            lineMirrors.push_back(d);
        }
    }

public:
    Mirrors(int noLineMirrors, int noCircleMirrors, int noSquareMirrors) {
        placeLineMirrors(noLineMirrors);
        placeCircleMirrors(noCircleMirrors);
        placeSquareMirrors(noSquareMirrors);
    }

    bool isOnLineMirror(const Vector2d *point, Vector2d *e1, Vector2d *e2, double *scale) {
        double px = point->x;
        double py = point->y;

        for (auto &line : lineMirrors) {

            Position start = line.getStart();
            double x1 = start.x;
            double y1 = start.y;

            Position end = line.getEnd();
            double x2 = end.x;
            double y2 = end.y;

            double lengthOfMirror = Vector2d(x1 - x2, y1 - y2).length();
            double distanceToOneEndpoint = Vector2d(px - x1, py - y1).length();
            double distanceToOtherEndpoint = Vector2d(px - x2, py - y2).length();

            double distanceThroughLightHead = distanceToOneEndpoint + distanceToOtherEndpoint;

            if (abs(lengthOfMirror - distanceThroughLightHead) <= MIRROR_APPROACH_MARGIN) {
                *e1 = Vector2d(line.getStart().x, line.getStart().y);
                *e2 = Vector2d(line.getEnd().x, line.getEnd().y);
                return true;
            }
            if (abs(lengthOfMirror - distanceThroughLightHead) < *scale) {
                *scale = abs(lengthOfMirror - distanceThroughLightHead);
            }
        }
        if (*scale < 1) { *scale = 1; }
        return false;
    }

    bool isOnCircleMirror(const Vector2d *point, Vector2d *center, double *scale) {
        double px = point->x;
        double py = point->y;

        for (auto &circle : circleMirrors) {
            double x1 = circle.getX();
            double y1 = circle.getY();

            double distanceFromCenter = Vector2d(x1 - px, y1 - py).length();
            if (distanceFromCenter - RADIUS_OF_CIRCLE <= MIRROR_APPROACH_MARGIN) {
                *center = Vector2d(circle.getX(), circle.getY());
                return true;
            }

            if (distanceFromCenter < *scale) { *scale = distanceFromCenter; }
        }

        if (*scale < 1) { *scale = 1; }
        return false;
    }
};

class LightPlay {
    constexpr static double RADIUS_OF_TARGET = 50;
    constexpr static double RADIUS_OF_SOURCE = 75;
    constexpr static double WINDOW_WIDTH = 1500;
    constexpr static double WINDOW_HEIGHT = 700;
    constexpr static double WINDOW_PADDING = 50;

    Color TARGET_COLOR_ON_HIT = COLOR(153, 0, 0);
    Color TARGET_COLOR_NORMAL = COLOR(0, 204, 0);
    Color INSTRUCTION_COLOR = COLOR(255, 255, 255);

    Light *light;
    Mirrors *mirrors;

    Rectangle border1, border2;
    Circle target;
    Circle src1, src2, src3, src4;
    Text instruction;

    double scale;

    void reachedTargetCallback() {
        light->newRay();
        target.setColor(TARGET_COLOR_ON_HIT);
        instruction.reset(170, 25, "Congratulations! You popped the balloon!");
        instruction.setColor(INSTRUCTION_COLOR);
        wait(2);
    }

    void reflectOnLine(const Vector2d *e1, const Vector2d *e2) {
        light->newRay();
        double a = light->getDirection();
        double o = lineAngle(e1->x, e1->y, e2->x, e2->y);
        double b = 2 * o - a;
        light->setDirection(b);
        light->moveHead(2);
    }

    void reflectOnCircle(const Vector2d *center) {
        light->newRay();
        double a = light->getDirection();
        Vector2d lightHead = light->getHead();
        double o1 = lineAngle(lightHead.x, lightHead.y, center->x, center->y);
        double o = o1 > 0 ? o1 - 90 : o1 + 90;
        double b = 2 * o - a;
        light->setDirection(b);
        light->moveHead(2);
    }

    void placeTarget() {
        instruction.reset(90, 25, "Place the balloon");
        instruction.setColor(INSTRUCTION_COLOR);
        Vector2d click;
        registerClick(&click);
        target = Circle(click.x, click.y, RADIUS_OF_TARGET);
        target.setColor(TARGET_COLOR_NORMAL).setFill();
    }

    Vector2d placeSource() {
        instruction.reset(90, 25, "Place the laser source");
        instruction.setColor(INSTRUCTION_COLOR);
        Vector2d click;
        registerClick(&click);
        src1.reset(click.x, click.y, RADIUS_OF_SOURCE);
        src1.setColor(COLOR(242, 17, 17)).imprint();
        src2.reset(click.x, click.y, RADIUS_OF_SOURCE * 0.75);
        src2.setColor(COLOR(242, 187, 17)).imprint();
        src3.reset(click.x, click.y, RADIUS_OF_SOURCE * 0.25);
        src3.setColor(COLOR(242, 137, 17)).imprint();
        src4.reset(click.x, click.y, RADIUS_OF_SOURCE * 0.05);
        src4.setColor(COLOR(24, 137, 17)).imprint();
        return click;
    }

    void drawWalls() {
        border1 = Rectangle(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT);
        border1.setColor(COLOR(65, 65, 65)).setFill();
        border1.imprint();
        border2 = Rectangle(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2,
                            WINDOW_WIDTH - 2 * WINDOW_PADDING,
                            WINDOW_HEIGHT - 2 * WINDOW_PADDING);
        border2.setColor(COLOR(0, 0, 0)).setFill();
        border2.imprint();
    }

    bool isOnWall(const Vector2d *point) {
        return point->x <= WINDOW_PADDING
               || point->y <= WINDOW_PADDING
               || point->x >= WINDOW_WIDTH - WINDOW_PADDING
               || point->y >= WINDOW_HEIGHT - WINDOW_PADDING;
    }

public:

    LightPlay(int noLineMirrors, int noCircleMirrors, int noSquareMirrors) {
        scale = 1;
        drawWalls();
        mirrors = new Mirrors(noLineMirrors, noCircleMirrors, noSquareMirrors);
        const Vector2d epoch = placeSource();
        light = new Light(epoch);
        placeTarget();

        instruction.reset(90, 25, "Pop the balloon");
        instruction.setColor(INSTRUCTION_COLOR);

        // single game
        while (true) {
            Vector2d click;
            Vector2d lightHead;

            // reset
            while (true) {
                registerClick(&click);
                if (Vector2d(click.x - epoch.x, click.y - epoch.y).length() <= 75) { break; }
            }
            light->reset();

            // light
            light->setDirection(rayAngle(epoch.x, epoch.y, click.x, click.y));
            while (true) {
                // ray
                while (true) {
                    light->moveHead(scale);
                    lightHead = light->getHead();
                    // hit target
                    if (Vector2d(lightHead.x - target.getX(), lightHead.y - target.getY()).length() <= 50) {
                        reachedTargetCallback();
                        return;
                    }
                    // hit wall
                    if (isOnWall(&lightHead)) {
                        light->newRay();
                        break;
                    }
                    // hit line mirror
                    Vector2d e1, e2;
                    if (mirrors->isOnLineMirror(&lightHead, &e1, &e2, &scale)) {
                        reflectOnLine(&e1, &e2);
                    }
                    // hit circle mirror
                    Vector2d center;
                    if (mirrors->isOnCircleMirror(&lightHead, &center, &scale)) {
                        reflectOnCircle(&center);
                    }
                }
                if (isOnWall(&lightHead)) { break; }
            }
        }
    }

    ~LightPlay() {
        delete light;
        delete mirrors;
    }

};