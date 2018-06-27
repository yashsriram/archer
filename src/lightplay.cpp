#include<simplecpp>
#include<vector>
#include "utils.cpp"

class LightPlay {
    int n; //no of vertices including the initial point
    double **c; //coordinate of leading point
    vector<Line> lightRays;

    double dx, dy;//translators

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

    void assigntoheap() {

        c = new double *[n];
        for (int i = 0; i < n; i++) {
            c[i] = new double[2];
        }
        return;
    }

    void removefromheap() {
        for (int i = 0; i < n; i++) { delete[]c[i]; }
        delete[]c;
        return;
    }

    void resetthesysduringcollision() {
        double swc[n][2];
        for (int i = 0; i < n; i++) {
            swc[i][0] = c[i][0];
            swc[i][1] = c[i][1];
        }
        //coordiantes copied
        removefromheap();
        n = n + 1;
        assigntoheap();
        for (int i = 0; i < n - 1; i++) {
            c[i][0] = swc[i][0];
            c[i][1] = swc[i][1];
        }
        c[n - 1][0] = c[n - 2][0];
        c[n - 1][1] = c[n - 2][1];

        return;
    }

    void ontarget() {
        Line lin(c[n - 2][0], c[n - 2][1], c[n - 1][0], c[n - 1][1]);
        lin.setColor(COLOR(255, 255, 255));
        lightRays.push_back(lin);
        target.setColor(COLOR(153, 0, 0));
        wait(2);
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

    bool on_mirror() {
        for (int i = 0; i < noLineMirrors + noSquareMirrors * 16; i++) {
            double a, b, x1, y1, x2, y2;
            a = c[n - 1][0];
            b = c[n - 1][1];
            x1 = lineMirrorEndpoints[i][0][0];
            y1 = lineMirrorEndpoints[i][0][1];
            x2 = lineMirrorEndpoints[i][1][0];
            y2 = lineMirrorEndpoints[i][1][1];


            double lenght = sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
            double a1 = sqrt((a - x1) * (a - x1) + (b - y1) * (b - y1)), b1 = sqrt(
                    (a - x2) * (a - x2) + (b - y2) * (b - y2));

            double sumOfparts = a1 + b1;

            if (abs(lenght - sumOfparts) <= 0.5) {
                pass = i;
                return true;
            }

            if (abs(lenght - sumOfparts) < scale) { scale = abs(lenght - sumOfparts); }
        }

        if (scale < 1) { scale = 1; }

        return false;
    }

    bool on_sphere() {
        double a, b, x1, y1;
        a = c[n - 1][0];
        b = c[n - 1][1];
        for (int i = 0; i < noCircleMirrors; i++) {
            x1 = circleMirrorCenters[i][0];
            y1 = circleMirrorCenters[i][1];

            double lenght = sqrt((x1 - a) * (x1 - a) + (y1 - b) * (y1 - b));
            if (lenght <= 75.5) {
                pass = i;
                return true;
            }

            if (lenght < scale) { scale = lenght; }
        }

        if (scale < 1) { scale = 1; }

        return false;
    }

    void collision(int m) {

        double o, a, b;
        a = rayAngle(c[n - 2][0], c[n - 2][1], c[n - 1][0], c[n - 1][1]);
        o = lineAngle(lineMirrorEndpoints[m][0][0], lineMirrorEndpoints[m][0][1], lineMirrorEndpoints[m][1][0],
                      lineMirrorEndpoints[m][1][1]);
        b = 2 * o - a;
        dx = cosine(b);
        dy = sine(b);
        resetthesysduringcollision();
        Line lin(c[n - 3][0], c[n - 3][1], c[n - 2][0], c[n - 2][1]);
        lin.setColor(COLOR(255, 255, 255));
        lightRays.push_back(lin);
        c[n - 1][0] = c[n - 1][0] + 2 * dx;
        c[n - 1][1] = c[n - 1][1] + 2 * dy;
        return;
    }

    void Scollision(int m) {
        Line lin(c[n - 2][0], c[n - 2][1], c[n - 1][0], c[n - 1][1]);
        lin.setColor(COLOR(255, 255, 255));
        lightRays.push_back(lin);
        double o1, o, a, b;
        a = rayAngle(c[n - 2][0], c[n - 2][1], c[n - 1][0], c[n - 1][1]);
        o1 = lineAngle(c[n - 1][0], c[n - 1][1], circleMirrorCenters[m][0], circleMirrorCenters[m][1]);
        if (o1 > 0) { o = o1 - 90; }
        else { o = o1 + 90; }
        b = 2 * o - a;
        dx = cosine(b);
        dy = sine(b);
        resetthesysduringcollision();
        c[n - 1][0] = c[n - 1][0] + 2 * dx;
        c[n - 1][1] = c[n - 1][1] + 2 * dy;
        //Circle flag(c[n-1][0],c[n-1][1],10);
        return;

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

        n = 2;
        assigntoheap();

        double x1, y1;
        registerClick(&click);
        c[1][0] = c[0][0] = x1 = click.x;
        c[1][1] = c[0][1] = y1 = click.y;

        // Light source
        Circle refer(c[0][0], c[1][1], 75);
        refer.setColor(COLOR(242, 17, 17)).imprint();

        refer.reset(c[0][0], c[1][1], 50);
        refer.setColor(COLOR(242, 187, 17)).imprint();

        refer.reset(c[0][0], c[1][1], 25);
        refer.setColor(COLOR(242, 137, 17)).imprint();

        refer.reset(c[0][0], c[1][1], 2);
        refer.setColor(COLOR(24, 137, 17)).imprint();


        int checker = 0;
        scale = 1;
        while (true) {
            registerClick(&click);
            if (Vector2d(click.x - x1, click.y - y1).length() <= 75) { break; }
        }

        bool exit;
        while (true) {
            exit = false;

            double o;
            o = rayAngle(x1, y1, click.x, click.y);
            dx = cosine(o);
            dy = sine(o);

            //initiator launched


            while (true) {
                while (true) {
                    //cout<<"i";

                    c[n - 1][0] = c[n - 1][0] + dx * scale;
                    c[n - 1][1] = c[n - 1][1] + dy * scale;
                    checker++;


                    if (sqrt((c[n - 1][0] - tarc[0]) * (c[n - 1][0] - tarc[0]) +
                             (c[n - 1][1] - tarc[1]) * (c[n - 1][1] - tarc[1])) <= 50) {
                        ontarget();
                        exit = true;
                        cout << "PASSED" << endl;
                        break;
                    }
                    if (on_mirror()) { collision(pass); }
                    if (on_sphere()) { Scollision(pass); }
                    if (c[n - 1][0] >= 1450 || c[n - 1][0] <= 50 || c[n - 1][1] >= 650 || c[n - 1][1] <= 50) {
                        break;
                    }

                }
                if (exit) { break; }
                if (c[n - 1][0] >= 1450 || c[n - 1][0] <= 50 || c[n - 1][1] >= 650 || c[n - 1][1] <= 50) {
                    Line lin(c[n - 2][0], c[n - 2][1], c[n - 1][0], c[n - 1][1]);
                    lin.setColor(COLOR(255, 255, 255));
                    lightRays.push_back(lin);
                    break;
                }

            }


            //travels in way checks any reflection
            //distance calculating fn and hence reftector
            //resetting the system
            //storing new point data into the corres variables
            //end the reflections and create lines
            if (exit) { break; }
            while (true) {
                registerClick(&click);
                if (Vector2d(click.x - x1, click.y - y1).length() <= 75) { break; }
            }

            {//resetting the system
                //Line zero(0,0,0,0);
                //for(size_t i=0;i<lightRays.size();i++){lightRays[i]=zero;}
                lightRays.resize(0);
                removefromheap();
                n = 2;
                assigntoheap();
                c[1][0] = c[0][0] = x1;
                c[1][1] = c[0][1] = y1;
            }


        }
        //cout<<checker;
    }

    ~LightPlay() { removefromheap(); }

};
