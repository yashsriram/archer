#include <simplecpp>
#include "utils.cpp"

class Mirrors {
    vector<Line> lineMirrors;
    vector<Circle> circleMirrors;

    void placeLineMirrors(int noLineMirrors) {
        double x1, y1, x2, y2;
        Vector2d click;
        for (int i = 0; i < noLineMirrors; i++) {

            registerClick(&click);
            x1 = click.x;
            y1 = click.y;

            registerClick(&click);
            x2 = click.x;
            y2 = click.y;

            Line newLineMirror = Line(x1, y1, x2, y2);
            newLineMirror.setColor(COLOR(160, 160, 160));
            lineMirrors.push_back(newLineMirror);
        }
    }

    void placeCircleMirrors(int noCircleMirrors) {
        Vector2d click;
        for (int i = 0; i < noCircleMirrors; i++) {
            registerClick(&click);
            Circle newCircleMirror = Circle(click.x, click.y, 75);
            newCircleMirror.setColor(COLOR(160, 160, 160)).setFill();
            circleMirrors.push_back(newCircleMirror);
        }
    }

    /**
     * Square mirror is nothing but set of 4 Line mirrors
     * */
    void placeSquareMirrors(int noSquareMirrors) {
        const double SIDE_LENGTH_OF_SQUARE = 50;
        double x1, y1;
        Vector2d click;
        for (int i = 0; i < noSquareMirrors; i++) {
            registerClick(&click);
            x1 = click.x;
            y1 = click.y;

            Line a = Line(
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2,
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2);
            a.setColor(COLOR(160, 160, 160));
            lineMirrors.push_back(a);

            Line b = Line(
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2,
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2);
            b.setColor(COLOR(160, 160, 160));
            lineMirrors.push_back(b);

            Line c = Line(
                    x1 - SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2,
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2);
            c.setColor(COLOR(160, 160, 160));
            lineMirrors.push_back(c);

            Line d = Line(
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 - SIDE_LENGTH_OF_SQUARE / 2,
                    x1 + SIDE_LENGTH_OF_SQUARE / 2,
                    y1 + SIDE_LENGTH_OF_SQUARE / 2);
            d.setColor(COLOR(160, 160, 160));
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

            if (abs(lengthOfMirror - distanceThroughLightHead) <= 0.5) {
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
            if (distanceFromCenter - 75 <= 0.5) {
                *center = Vector2d(circle.getX(), circle.getY());
                return true;
            }

            if (distanceFromCenter < *scale) { *scale = distanceFromCenter; }
        }

        if (*scale < 1) { *scale = 1; }
        return false;
    }
};
