#ifndef MYMATH_H
#define MYMATH_H

#include <QVector>
#include <QPoint>
#include <QMap>

#include "polygon.h"

#define abs(x) ((x) < 0 ? -(x) : (x))
const int INF = ~0U >> 1;

enum IntersectionType {
    IN, OUT, NORMAL
};

enum Location {
    MAIN, CUT
};

struct Point {
    int x, y;
    Point() {}
    Point(QPoint qPoint) {
        x = qPoint.x();
        y = qPoint.y();
    }
    Point(int x, int y): x(x), y(y) {}
    Point operator * (const double &t) {
        return Point(x * t, y * t);
    }
    Point operator + (const Point &B) {
        return Point(x + B.x, y + B.y);
    }
    Point operator - (const Point &B) {
        return Point(x - B.x, y - B.y);
    }
    bool operator < (const Point &B) const {
        return x == B.x ? y < B.y : x < B.x;
    }
};
typedef Point Vector;

struct Line {
    Point p;
    Vector v;
    Line() {}
    Line(Point p1, Point p2): p(p1), v(p2 - p1) {}
};

struct Intersection {
    Point p;
    IntersectionType type;
    bool visited = false;
    int squareDistance = 0;

    Intersection() {}
    Intersection(Point p, IntersectionType type): p(p), type(type) {}

    bool operator < (const Intersection &B) const {
        return squareDistance < B.squareDistance;
    }
};

class MyMath {
    private:
        int pointCnt;
        Point startPoint;
        QMap<Point, int> pointIdMap;
        QMap<int, Intersection> idIntersectionMap;
        QMap<Point, Point> mainListMap, cutListMap;
        QVector<QVector<Intersection>> intersectionLists;
        QVector<QVector<int>> mainLists, cutLists;

        void resetContainers();
        void initContainers(Polygon &mainPolygon, Polygon &cutPolygon);
        void GetIntersections(Polygon &mainPolygon, Polygon &cutPolygon);
        bool compareFunction(const Intersection &A, const Intersection &B);
        void SortIntersections(Polygon &mainPolygon, Polygon &cutPolygon);
        void SortIntersection(int pointId);
        QPair<Location, Point> NextPoint(Location location, Point coordinate);
        void Dfs(Polygon &result, Location location, Point coordinate);
        Polygon Calculate(Polygon &mainPolygon);

        void PrintAllPoint(Polygon &mainPolygon, Polygon &cutPolygon);

    public:
        Polygon polygonIntersection(Polygon &mainPolygon, Polygon &cutPolygon);
};

#endif // MYMATH_H
