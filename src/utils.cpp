#include "vector2d.cpp"
#include <simplecpp>

/**
* Stores the coordinates of click into a vector2d
* */
void registerClick(Vector2d *vector2d) {
    const int twoPower16 = 65536;
    int point;
    point = getClick();
    int x = point / twoPower16;
    int y = point % twoPower16;
    vector2d->set(x, y);
}


/**
 * Returns the angle made by ray with X-axis in degrees
 * the order of the args matter
 * output range from 0 to 359
 * */
double rayAngle(double x1, double y1, double x2, double y2) {
    if (x2 - x1 == 0) {
        if (y2 - y1 > 0) { return 90; }
        else if (y2 - y1 < 0) { return 270; }
        else { return 0; }
    } else {
        double cosineOfAngle = (x2 - x1) / Vector2d(x2 - x1, y2 - y1).length();
        double angle = arccosine(cosineOfAngle);
        if (y2 - y1 >= 0) { return angle; }
        else { return 360 - angle; }
    }
}

/**
 * Returns the angle made by ray with X-axis in degrees
 * the order of the args matter
 * output range from 0 to 359
 * */
double rayAngle(const Vector2d *a, const Vector2d *b) {
    return rayAngle(a->x, a->y, b->x, b->y);
}

double vectorangle(double x1, double y1, double x2, double y2) {
    double cosineofangle, angle;
    if ((x2 - x1) != 0) {
        cosineofangle = ((x2 - x1) / sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));
        angle = arccosine(cosineofangle);
        if (y2 - y1 >= 0)return angle;
        if (y2 - y1 < 0)return -angle;
    } else {
        if (y2 - y1 > 0) return 90;
        if (y2 - y1 < 0) return -90;
    }
    return 0;
}

//(the order of input matters) OUTPUT -pi to pi
double lineangle(double x1, double y1, double x2, double y2) {
    if ((x2 - x1) == 0)return 90;

    double tangentofangle, angle;
    tangentofangle = ((y2 - y1) / (x2 - x1));
    angle = arctangent(tangentofangle);
    return angle;


}
//OUTPUT -pi/2 to pi/2
