#include "utils.cpp"
#include "light.cpp"
#include "mirrors.cpp"

class LightPlay {
    Light *light;
    Mirrors *mirrors;

    Circle target;
    Circle src1, src2, src3, src4;

    double scale;

    void reachedTargetCallback() {
        light->newRay();
        target.setColor(COLOR(153, 0, 0));
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
        Vector2d click;
        registerClick(&click);
        target = Circle(click.x, click.y, 50);
        target.setColor(COLOR(0, 204, 0)).setFill();
    }

    Vector2d placeSource() {
        Vector2d click;
        registerClick(&click);
        src1.reset(click.x, click.y, 75);
        src1.setColor(COLOR(242, 17, 17)).imprint();
        src2.reset(click.x, click.y, 50);
        src2.setColor(COLOR(242, 187, 17)).imprint();
        src3.reset(click.x, click.y, 25);
        src3.setColor(COLOR(242, 137, 17)).imprint();
        src4.reset(click.x, click.y, 2);
        src4.setColor(COLOR(24, 137, 17)).imprint();
        return click;
    }

public:

    LightPlay(int noLineMirrors, int noCircleMirrors, int noSquareMirrors) {
        scale = 1;
        mirrors = new Mirrors(noLineMirrors, noCircleMirrors, noSquareMirrors);
        const Vector2d epoch = placeSource();
        light = new Light(epoch);
        placeTarget();

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
                    if (light->isHeadOnWall()) {
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
                if (light->isHeadOnWall()) { break; }
            }
        }
    }

    ~LightPlay() {
        delete light;
        delete mirrors;
    }
};
