/*
 * anomaly_detection_util.cpp
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#include "anomaly_detection_util.h"
#include <math.h>

//Returns the expected value of x by calculating the average of the x values.
float expectedValue(float *x, int size) {
    float sum = 0;
    for (int i = 0; i < size; ++i) {
        sum += *(x + i);
    }
    return sum / size;
}

//returns the variance of the x values.
float var(float *x, int size) {
    float *pow2X = new float[size];
    float e = expectedValue(x, size);
    float powedX;
    for (int i = 0; i < size; ++i) {
        *(pow2X + i) = (*(x + i)) * (*(x + i));
    }
    powedX = expectedValue(pow2X, size);
    delete[] pow2X;
    return ((powedX) - e * e);
}

//Returns the covariance of the x and y values.
float cov(float *x, float *y, int size) {

    //Creating an array of the results of multiplying corresponding x and y values.
    float *multipliedXY = new float[size];
    for (int i = 0; i < size; ++i) {
        *(multipliedXY + i) = (*(x + i)) * (*(y + i));
    }
    float covariance = expectedValue(multipliedXY, size) - expectedValue(x, size) * expectedValue(y, size);
    delete[] multipliedXY;
    return covariance;
}

//Returns the Pearson correlation coefficient of the x and y vales.
float pearson(float *x, float *y, int size) {
    float covariance = cov(x, y, size);
    float deviation = sqrt(var(x, size) * var(y, size));
    return covariance / deviation;
}

//performs a linear regression on the given points and returns the line equation.
Line linear_reg(Point **points, int size) {
    float *x = new float[size];
    float *y = new float[size];
    for (int i = 0; i < size; ++i) {
        *(x + i) = (*(*(points + i))).x;
        *(y + i) = (*(*(points + i))).y;
    }
    float a = cov(x, y, size) / var(x, size);
    float b = expectedValue(y, size) - a * expectedValue(x, size);
    delete[] x;
    delete[] y;
    return Line(a, b);
}

//Returns the deviation between point p and the line equation of the points..
float dev(Point p, Point **points, int size) {
    Line line = linear_reg(points, size);
    float difference = line.f(p.x) - p.y;
    if (difference <= 0) {
        return -difference;
    }
    return difference;
}

//Returns the deviation between point p and the line.
float dev(Point p, Line l) {
    float difference = l.f(p.x) - p.y;
    if (difference <= 0) {
        return -difference;
    }
    return difference;
}
