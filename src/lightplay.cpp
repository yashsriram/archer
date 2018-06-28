#include<simplecpp>
#include "utils.cpp"

class Light {
    vector<Line> rays;
    const Vector2d *tail;
    Vector2d *head;
    double direction;
    double dx, dy;
public:
    Light(Vector2d *epoch) : tail(new Vector2d(epoch)),
                             head(new Vector2d(epoch)),
                             direction(0),
                             dx(cosine(0)),
                             dy(sine(0)) {}

    ~Light() {
        delete head;
        delete tail;
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
            Line line(tail->x, tail->y, head->x, head->y);
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

    void reset() {
        rays.clear();
        head->set(tail);
        direction = dx = dy = 0;
    }
};

class LightPlay {
    Light *light;

    double lineMirrorEndpoints[200][2][2]; //mirrors end points data******100 at max
    double circleMirrorCenters[100][2];

    int noLineMirrors;
    vector<Line> lineMirrors;
    int noCircleMirrors;
    vector<Circle> circleMirrors;
    int noSquareMirrors;
    vector<Rectangle> squareMirrors;

    double tarc[2];
    Circle target;

    int pass;
    double scale;

    void reachedTargetCallback() {
        light->newRay();
        target.setColor(COLOR(153, 0, 0));
        wait(2);
    }

    bool isOnLineMirror() {
        for (int i = 0; i < noLineMirrors + noSquareMirrors * 16; i++) {

            Vector2d lightHead = light->getHead();
            double lhx = lightHead.x;
            double lhy = lightHead.y;

            double x1 = lineMirrorEndpoints[i][0][0];
            double y1 = lineMirrorEndpoints[i][0][1];

            double x2 = lineMirrorEndpoints[i][1][0];
            double y2 = lineMirrorEndpoints[i][1][1];

            double lengthOfMirror = Vector2d(x1 - x2, y1 - y2).length();
            double distanceToOneEndpoint = Vector2d(lhx - x1, lhy - y1).length();
            double distanceToOtherEndpoint = Vector2d(lhx - x2, lhy - y2).length();

            double distanceThroughLightHead = distanceToOneEndpoint + distanceToOtherEndpoint;

            if (abs(lengthOfMirror - distanceThroughLightHead) <= 0.5) {
                pass = i;
                return true;
            }
            if (abs(lengthOfMirror - distanceThroughLightHead) < scale) {
                scale = abs(lengthOfMirror - distanceThroughLightHead);
            }
        }
        if (scale < 1) { scale = 1; }
        return false;
    }

    bool isOnCircleMirror() {
        Vector2d lightHead = light->getHead();
        const double lhx = lightHead.x;
        const double lhy = lightHead.y;

        for (int i = 0; i < noCircleMirrors; i++) {
            double x1 = circleMirrorCenters[i][0];
            double y1 = circleMirrorCenters[i][1];

            double distanceFromCenter = Vector2d(x1 - lhx, y1 - lhy).length();
            if (distanceFromCenter <= 75.5) {
                pass = i;
                return true;
            }

            if (distanceFromCenter < scale) { scale = distanceFromCenter; }
        }

        if (scale < 1) { scale = 1; }
        return false;
    }

    void reflectOnLine(int lineMirror) {
        light->newRay();
        double a = light->getDirection();
        double o = lineAngle(lineMirrorEndpoints[lineMirror][0][0],
                             lineMirrorEndpoints[lineMirror][0][1],
                             lineMirrorEndpoints[lineMirror][1][0],
                             lineMirrorEndpoints[lineMirror][1][1]);
        double b = 2 * o - a;
        light->setDirection(b);
        light->moveHead(2);
    }

    void reflectOnCircle(int circleMirror) {
        light->newRay();
        double a = light->getDirection();
        Vector2d lightHead = light->getHead();
        double o1 = lineAngle(lightHead.x, lightHead.y,
                              circleMirrorCenters[circleMirror][0],
                              circleMirrorCenters[circleMirror][1]);
        double o = o1 > 0 ? o1 - 90 : o1 + 90;
        double b = 2 * o - a;
        light->setDirection(b);
        light->moveHead(2);
    }

    void placeLineMirrors() {
        double x1, y1, x2, y2;
        Vector2d click;
        for (int i = 0; i < noLineMirrors; i++) {
            registerClick(&click);
            lineMirrorEndpoints[i][0][0] = x1 = click.x;
            lineMirrorEndpoints[i][0][1] = y1 = click.y;

            registerClick(&click);
            lineMirrorEndpoints[i][1][0] = x2 = click.x;
            lineMirrorEndpoints[i][1][1] = y2 = click.y;

            Line newLineMirror = Line(x1, y1, x2, y2);
            newLineMirror.setColor(COLOR(160, 160, 160));
            lineMirrors.push_back(newLineMirror);
        }
    }

    void placeCircleMirrors() {
        double x1, y1;
        Vector2d click;
        for (int i = 0; i < noCircleMirrors; i++) {
            registerClick(&click);
            circleMirrorCenters[i][0] = x1 = click.x;
            circleMirrorCenters[i][1] = y1 = click.y;

            Circle newCircleMirror = Circle(x1, y1, 75);
            newCircleMirror.setColor(COLOR(160, 160, 160)).setFill();
            circleMirrors.push_back(newCircleMirror);
        }
    }

    /**
     * Square mirror is nothing but set of 4 Line mirrors
     * */
    void placeSquareMirrors() {
        int corr = 0;
        const double SIDE_LENGTH_OF_SQUARE = 50;
        double x1, y1;
        Vector2d click;
        for (int i = 0; i < noSquareMirrors; i++) {

            registerClick(&click);
            x1 = click.x;
            y1 = click.y;

            lineMirrorEndpoints[noLineMirrors + i + corr][0][0] = x1 + SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + corr][0][1] = y1 + SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + corr][1][0] = x1 - SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + corr][1][1] = y1 + SIDE_LENGTH_OF_SQUARE / 2;

            lineMirrorEndpoints[noLineMirrors + i + 1 + corr][0][0] = x1 - SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 1 + corr][0][1] = y1 + SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 1 + corr][1][0] = x1 - SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 1 + corr][1][1] = y1 - SIDE_LENGTH_OF_SQUARE / 2;

            lineMirrorEndpoints[noLineMirrors + i + 2 + corr][0][0] = x1 - SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 2 + corr][0][1] = y1 - SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 2 + corr][1][0] = x1 + SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 2 + corr][1][1] = y1 - SIDE_LENGTH_OF_SQUARE / 2;

            lineMirrorEndpoints[noLineMirrors + i + 3 + corr][0][0] = x1 + SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 3 + corr][0][1] = y1 - SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 3 + corr][1][0] = x1 + SIDE_LENGTH_OF_SQUARE / 2;
            lineMirrorEndpoints[noLineMirrors + i + 3 + corr][1][1] = y1 + SIDE_LENGTH_OF_SQUARE / 2;
            corr = corr + 3;

            Rectangle newSquareMirror = Rectangle(x1, y1, SIDE_LENGTH_OF_SQUARE, SIDE_LENGTH_OF_SQUARE);
            newSquareMirror.setColor(COLOR(160, 160, 160)).imprint();
            squareMirrors.push_back(newSquareMirror);
        }

    }

    void placeTarget() {
        Vector2d click;
        registerClick(&click);
        tarc[0] = click.x;
        tarc[1] = click.y;
        target = Circle(tarc[0], tarc[1], 50);
        target.setColor(COLOR(0, 204, 0)).setFill();
    }

public:

    LightPlay(int noLineMirrors, int noCircleMirrors, int noSquareMirrors) :
            noLineMirrors(noLineMirrors),
            noCircleMirrors(noCircleMirrors),
            noSquareMirrors(noSquareMirrors) {
        placeLineMirrors();
        placeCircleMirrors();
        placeSquareMirrors();
        placeTarget();

        Vector2d click;

        registerClick(&click);
        Vector2d epoch = Vector2d(click.x, click.y);
        light = new Light(&epoch);

        // Source
        Circle src1(epoch.x, epoch.y, 75);
        src1.setColor(COLOR(242, 17, 17)).imprint();
        Circle src2(epoch.x, epoch.y, 50);
        src2.setColor(COLOR(242, 187, 17)).imprint();
        Circle src3(epoch.x, epoch.y, 25);
        src3.setColor(COLOR(242, 137, 17)).imprint();
        Circle src4(epoch.x, epoch.y, 2);
        src4.setColor(COLOR(24, 137, 17)).imprint();

        int checker = 0;
        scale = 1;
        while (true) {
            registerClick(&click);
            if (Vector2d(click.x - epoch.x, click.y - epoch.y).length() <= 75) { break; }
        }
        bool exit;
        // single game
        while (true) {
            exit = false;
            double o;
            o = rayAngle(epoch.x, epoch.y, click.x, click.y);
            light->setDirection(o);
            Vector2d lightHead;
            // single walk
            while (true) {

                // single ray
                while (true) {
                    //cout<<"i";

                    light->moveHead(scale);
                    checker++;
                    lightHead = light->getHead();
                    if (Vector2d(lightHead.x - tarc[0], lightHead.y - tarc[1]).length() <= 50) {
                        reachedTargetCallback();
                        exit = true;
                        cout << "PASSED" << endl;
                        break;
                    }
                    if (isOnLineMirror()) { reflectOnLine(pass); }
                    if (isOnCircleMirror()) { reflectOnCircle(pass); }
                    lightHead = light->getHead();
                    if (lightHead.x >= 1450 || lightHead.x <= 50 || lightHead.y >= 650 || lightHead.y <= 50) { break; }
                }
                if (exit) { break; }
                if (lightHead.x >= 1450 || lightHead.x <= 50 || lightHead.y >= 650 || lightHead.y <= 50) {
                    light->newRay();
                    break;
                }

            }

            //travels in way checks any reflection
            //distance calculating fn and hence reftector
            //resetting the system
            //storing new_ point data into the corres variables
            //end the reflections and create lines
            if (exit) { break; }

            while (true) {
                registerClick(&click);
                if (Vector2d(click.x - epoch.x, click.y - epoch.y).length() <= 75) { break; }
            }

            //resetting the system
            //Line zero(0,0,0,0);
            //for(size_t i=0;i<lightRays.size();i++){lightRays[i]=zero;}
            light->reset();
        }
        //cout<<checker;
    }

    ~LightPlay() {
        delete light;
    }

};
