/*
 * minCircle.cpp
 *
 * Author: Ofek Koren - 207105933, Tomer Aligaev - 208668129
 */

#include "minCircle.h"

bool isInside(Circle &circle, Point &point) {
    Point center = circle.center;
    float dis = distance(center, point);
    //If the distance of the point from the center of the circle is smaller than the radius it's inside the circle.
    if (dis <= circle.radius)
        return true;
    return false;
}

Circle createCircleFromTwoPoints(Point &p1, Point &p2) {

    //The center of the circle is the middle point between a and b.
    float newX = (p1.x + p2.x) / FLOAT_TWO;
    float newY = (p1.y + p2.y) / FLOAT_TWO;
    Point center = Point(newX, newY);
    //The radius is half of the distance between a and b.
    float newRadius = distance(p1, p2) / FLOAT_TWO;
    return Circle(center, newRadius);
}

Circle createCircleFromThreePoints(Point &p1, Point &p2, Point &p3) {

    //The center of the circle is the center of the circumcircle of the triangle described by the points.
    float baX = p2.x - p1.x;
    float baY = p2.y - p1.y;
    float caX = p3.x - p1.x;
    float caY = p3.y - p1.y;
    float b = baX * baX + baY * baY;
    float c = caX * caX + caY * caY;
    float d = baX * caY - baY * caX;
    Point newCenter = Point((caY * b - baY * c) / (FLOAT_TWO * d), (baX * c - caX * b) / (FLOAT_TWO * d));
    newCenter.x += p1.x;
    newCenter.y += p1.y;
    //All the points lay on the circle, the radius will be the distance of one of the points from the center.
    return Circle(newCenter, distance(newCenter, p1));
}

Circle edgeCaseCircle(vector<Point> &points) {
    if (points.empty()) {
        //Returning the point (0,0) with radius 0 if there are zero points.
        return (Circle(Point(0, 0), 0));
    } else if (points.size() == 1) {
        //If the is one point we return a circle with radius 0 and the point itself as the center.
        return (Circle(points[0], 0));
    } else if (points.size() == 2) {
        return createCircleFromTwoPoints(points[0], points[1]);
    }
    //Checking if the circle enclosing two points includes the third point.
    Circle c = createCircleFromTwoPoints(points[0], points[1]);
    if (distance(points[2], c.center) <= c.radius)
        return c;
    c = createCircleFromTwoPoints(points[0], points[2]);
    if (distance(points[1], c.center) <= c.radius)
        return c;
    c = createCircleFromTwoPoints(points[1], points[2]);
    if (distance(points[0], c.center) <= c.radius)
        return c;
    //Returning a circle with the three points on it's bound.
    return createCircleFromThreePoints(points[0], points[1], points[2]);
}

Circle auxFindMinCircle(Point **points, vector<Point> pointsInsideCir, int numOfPointsP) {

    /*
     * The stopping condition is if the points array is empty or when there are 3 points in pointsInsideCir which is
     * the maximal number of points that can define an enclosing circle in a trivial way.
     */
    if (numOfPointsP == 0 || pointsInsideCir.size() == 3) {
        return edgeCaseCircle(pointsInsideCir);
    }
    //Picking a random point from the array.
    int indexOfPointInP = rand() % numOfPointsP;
    Point *point = points[indexOfPointInP];
    //Putting the picked point at the end the array instead of deleting it from the array.
    Point *temp = points[indexOfPointInP];
    points[indexOfPointInP] = points[numOfPointsP - 1];
    points[numOfPointsP - 1] = temp;
    //Getting the minimal enclosing circle for all points in points with points from pointsInsideCir on it's bound.
    Circle circle = auxFindMinCircle(points, pointsInsideCir, numOfPointsP - 1);
    //If the picked point is in the circle it is returned.
    if (isInside(circle, *point)) {
        return circle;
    }

    /*
     * Else,We push the picked point to the vector of points that should be on the bound and returning the minimal
     * enclosing circle for the points in the array without the picked point (but the picked point is on the bound).
     */
    pointsInsideCir.push_back(*point);
    return auxFindMinCircle(points, pointsInsideCir, numOfPointsP - 1);
}

Circle findMinCircle(Point **points, size_t size) {
    //random_shuffle(std::begin(points), std::end(points));
    return auxFindMinCircle(points, {}, size);
}

float distance(Point &a, Point &b) {
    return sqrt((a.getX() - b.getX()) * (a.getX() - b.getX()) + (a.getY() - b.getY()) * (a.getY() - b.getY()));
}