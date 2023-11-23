/*
 * minCircle.h
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#ifndef MINCIRCLE_H_
#define MINCIRCLE_H_

#include <iostream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>
#include "anomaly_detection_util.h"

//The macro constant will be used to avoid magic numbers in the code
#define FLOAT_TWO 2.0

using namespace std;

// ------------ DO NOT CHANGE -----------

class Circle {
public:
    Point center;
    float radius;

    //Basic constructor that builds a circle with radius r and center point c.
    Circle(Point c, float r) : center(c), radius(r) {}
};
// --------------------------------------

//Returning the minimal enclosing circle for all points in "points".
Circle findMinCircle(Point **points, size_t size);

/*
 * An auxiliary function for findMinCircle that impements the welzl algorithm to find minimal enclosing circle for
 * the points in "points" array.
 * Point **points - Points that should be included inside the minimal enclosing circle.
 * vector<Point> pointsInsideCir - Points that should be on the bound of the minimal circle. It's maximal size is 3.
 * int numOfPointsP - number of points in points array.
 */
Circle auxFindMinCircle(Point **points, vector<Point> pointsInsideCir, int numOfPointsP);

//The function returns true the point is inside the circle. else, false is returned.
bool isInside(Circle &circle, Point &point);

//The function returns the smallest circle that intersects with points p1 and p2.
Circle createCircleFromTwoPoints(Point &p1, Point &p2);

//The function returns the smallest circle that intersects with points p1,p2 and p3.
Circle createCircleFromThreePoints(Point &p1, Point &p2, Point &p3);

/*
 * The method return the minimal enclosing circle that has all the points in vector "points" on it's bound. The
 * maximal size of points is 3 so that finding a minimal enclosing circle that have the points on it's bound is
 * trivial and can be done in a fixed size of steps.
 */
Circle edgeCaseCircle(vector<Point> &points);

//Return the Euclidean distance between points a and b.
float distance(Point &a, Point &b);

#endif /* MINCIRCLE_H_ */
