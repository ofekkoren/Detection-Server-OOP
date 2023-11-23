/*
 * anomaly_detection_util.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef MAIN_CPP_ANOMALY_DETECTION_UTIL_H
#define MAIN_CPP_ANOMALY_DETECTION_UTIL_H

#include <vector>


// returns the variance of X and y
float var(float *x, int size);

// returns the covariance of X and y
float cov(float *x, float *y, int size);

// returns the Pearson correlation coefficient of X and y
float pearson(float *x, float *y, int size);

class Line {
public:
    float a, b;

    Line() : a(0), b(0) {}

    Line(float a, float b) : a(a), b(b) {}

    float f(float x) {
        return a * x + b;
    }
};

class Point {
public:
    float x, y;

    Point(float x, float y) : x(x), y(y) {}

    float getX() {
        return this->x;
    }

    float getY() {
        return this->y;
    }

    void setX(float newX) {
        this->x = newX;
    }

    void setY(float newY) {
        this->y = newY;
    }


};

// performs a linear regression and return s the line equation
Line linear_reg(Point **points, int size);

// returns the deviation between point p and the line equation of the points
float dev(Point p, Point **points, int size);

// returns the deviation between point p and the line
float dev(Point p, Line l);

#endif //MAIN_CPP_ANOMALY_DETECTION_UTIL_H
