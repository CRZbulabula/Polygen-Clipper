#include "myMath.h"

int Dot(Vector v1, Vector v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

int Cross(Vector v1, Vector v2)
{
    return v1.x * v2.y - v2.x * v1.y;
}

int SquareDistance(Point p1, Point p2)
{
    return Dot(p2 - p1, p2 - p1);
}

bool OnLeft(Line l, Point p)
{
    return Cross(l.v, p - l.p) < 0;
}

Point GetIntersection(Line L1, Line L2)
{
    Vector u = L1.p - L2.p;
    int up = Cross(L2.v, u);
    int down = Cross(L1.v, L2.v);
    if (down == 0) {
        return Point(-INF, -INF);
    }
    double t = (double) up / (double) down;
    if (t <= 0 || t >= 1) {
        return Point(-INF, -INF);
    }

    u = L2.p - L1.p;
    up = Cross(L1.v, u);
    down = Cross(L2.v, L1.v);
    if (down == 0) {
        return Point(-INF, -INF);
    }
    double _t = (double) up / (double) down;
    if (_t <= 0 || _t >= 1) {
        return Point(-INF, -INF);
    }

    return L1.p + L1.v * t;
}

int NextPointId(int id, int size)
{
    return id == size - 1 ? 0 : id + 1;
}

void MyMath::resetContainers()
{
    pointCnt = 0;
    idIntersectionMap.clear();
    intersectionLists.clear();
    mainLists.clear();
    cutLists.clear();
    mainListMap.clear();
    cutListMap.clear();
}

void MyMath::initContainers(Polygon &mainPolygon, Polygon &cutPolygon)
{
    for (int subPolygonId = 0; subPolygonId < mainPolygon.subPolygons.size(); subPolygonId++) {
        for (int subPointId = 0; subPointId < mainPolygon.subPolygons[subPolygonId].size(); subPointId++) {
            pointIdMap.insert(mainPolygon.subPolygons[subPolygonId][subPointId], pointCnt);
            idIntersectionMap.insert(pointCnt++, Intersection(mainPolygon.subPolygons[subPolygonId][subPointId], NORMAL));
        }
    }
    for (int subPolygonId = 0; subPolygonId < cutPolygon.subPolygons.size(); subPolygonId++) {
        for (int subPointId = 0; subPointId < cutPolygon.subPolygons[subPolygonId].size(); subPointId++) {
            pointIdMap.insert(cutPolygon.subPolygons[subPolygonId][subPointId], pointCnt);
            idIntersectionMap.insert(pointCnt++, Intersection(cutPolygon.subPolygons[subPolygonId][subPointId], NORMAL));
        }
    }
}

void MyMath::GetIntersections(Polygon &mainPolygon, Polygon &cutPolygon)
{
    for (int pointId = 0; pointId < pointCnt; pointId++) {
        QVector<Intersection> newVector;
        intersectionLists.push_back(newVector);
    }
    for (int mainSubPolygonId = 0; mainSubPolygonId < mainPolygon.subPolygons.size(); mainSubPolygonId++) {
        QVector<QPoint> &mainSubPolygon = mainPolygon.subPolygons[mainSubPolygonId];
        for (int cutSubPolygonId = 0; cutSubPolygonId < cutPolygon.subPolygons.size(); cutSubPolygonId++) {
            QVector<QPoint> &cutSubPolygon = cutPolygon.subPolygons[cutSubPolygonId];
            for (int mainSubPointId = 0; mainSubPointId < mainSubPolygon.size(); mainSubPointId++){
                int mainPointId = *pointIdMap.find(mainSubPolygon[mainSubPointId]);
                Line mainLine = Line(mainSubPolygon[mainSubPointId], mainSubPolygon[NextPointId(mainSubPointId, mainSubPolygon.size())]);
                for (int cutSubPointId = 0; cutSubPointId < cutSubPolygon.size(); cutSubPointId++) {
                    int cutPointId = *pointIdMap.find(cutSubPolygon[cutSubPointId]);
                    Line cutLine = Line(cutSubPolygon[cutSubPointId], cutSubPolygon[NextPointId(cutSubPointId, cutSubPolygon.size())]);
                    Point intersection = GetIntersection(cutLine, mainLine);
                    if (intersection.x == -INF && intersection.y == -INF) {
                        continue;
                    }
                    pointIdMap.insert(intersection, pointCnt);

                    // When the starting point of the main polygon edge is to the left of the cut polygon edge,
                    // the intersection is the OUT point; otherwise, the IN point
                    if (OnLeft(cutLine, mainSubPolygon[mainSubPointId])) {
                        idIntersectionMap.insert(pointCnt, Intersection(intersection, OUT));
                        intersectionLists[mainPointId].push_back(Intersection(intersection, OUT));
                        intersectionLists[cutPointId].push_back(Intersection(intersection, OUT));
                    } else {
                        idIntersectionMap.insert(pointCnt, Intersection(intersection, IN));
                        intersectionLists[mainPointId].push_back(Intersection(intersection, IN));
                        intersectionLists[cutPointId].push_back(Intersection(intersection, IN));
                    }
                    ++pointCnt;
                }
            }
        }
    }
}

void MyMath::SortIntersection(int pointId)
{
    QVector<Intersection> &intersectionList = intersectionLists[pointId];
    for (int intersectionId = 0; intersectionId < intersectionList.size(); intersectionId++) {
        Intersection curIntersection = intersectionList[intersectionId];
        intersectionList[intersectionId].squareDistance = SquareDistance(startPoint, curIntersection.p);
    }
    qSort(intersectionList.begin(), intersectionList.end());
}

void MyMath::SortIntersections(Polygon &mainPolygon, Polygon &cutPolygon)
{
    for (int subPolygonId = 0; subPolygonId < mainPolygon.subPolygons.size(); subPolygonId++) {
        QVector<int> mainList;
        for (int subPointId = 0; subPointId < mainPolygon.subPolygons[subPolygonId].size(); subPointId++) {
            startPoint = mainPolygon.subPolygons[subPolygonId][subPointId];
            int pointId = *pointIdMap.find(startPoint);
            SortIntersection(pointId);

            mainListMap.insert(startPoint, Point(subPolygonId, mainList.size()));
            mainList.push_back(pointId);

            QVector<Intersection> &intersectionList = intersectionLists[pointId];
            for (int intersectionId = 0; intersectionId < intersectionList.size(); intersectionId++) {
                mainListMap.insert(intersectionList[intersectionId].p, Point(subPolygonId, mainList.size()));
                mainList.push_back(*pointIdMap.find(intersectionList[intersectionId].p));
            }
        }
        mainLists.push_back(mainList);
    }
    for (int subPolygonId = 0; subPolygonId < cutPolygon.subPolygons.size(); subPolygonId++) {
        QVector<int> cutList;
        for (int subPointId = 0; subPointId < cutPolygon.subPolygons[subPolygonId].size(); subPointId++) {
            startPoint = cutPolygon.subPolygons[subPolygonId][subPointId];
            int pointId = *pointIdMap.find(startPoint);
            SortIntersection(pointId);

            cutListMap.insert(startPoint, Point(subPolygonId, cutList.size()));
            cutList.push_back(pointId);

            QVector<Intersection> &intersectionList = intersectionLists[pointId];
            for (int intersectionId = 0; intersectionId < intersectionList.size(); intersectionId++) {
                cutListMap.insert(intersectionList[intersectionId].p, Point(subPolygonId, cutList.size()));
                cutList.push_back(*pointIdMap.find(intersectionList[intersectionId].p));
            }
        }
        cutLists.push_back(cutList);
    }
}

QPair<Location, Point> MyMath::NextPoint(Location location, Point coordinate)
{
    if (location == MAIN) {
        Intersection intersection = *idIntersectionMap.find(mainLists[coordinate.x][coordinate.y]);
        if (intersection.type == OUT) {
            Point other = *cutListMap.find(intersection.p);
            if (other.y == cutLists[other.x].size() - 1) {
                return QPair<Location, Point>(CUT, Point(other.x, 0));
            } else {
                return QPair<Location, Point>(CUT, Point(other.x, other.y + 1));
            }
        } else {
            if (coordinate.y == mainLists[coordinate.x].size() - 1) {
                return QPair<Location, Point>(location, Point(coordinate.x, 0));
            } else {
                return QPair<Location, Point>(location, Point(coordinate.x, coordinate.y + 1));
            }
        }
    } else {
        Intersection intersection = *idIntersectionMap.find(cutLists[coordinate.x][coordinate.y]);
        if (intersection.type == IN) {
            Point other = *mainListMap.find(intersection.p);
            if (other.y == mainLists[other.x].size() - 1) {
                return QPair<Location, Point>(MAIN, Point(other.x, 0));
            } else {
                return QPair<Location, Point>(MAIN, Point(other.x, other.y + 1));
            }
        } else {
            if (coordinate.y == cutLists[coordinate.x].size() - 1) {
                return QPair<Location, Point>(location, Point(coordinate.x, 0));
            } else {
                return QPair<Location, Point>(location, Point(coordinate.x, coordinate.y + 1));
            }
        }

    }
}

void MyMath::Dfs(Polygon &result, Location location, Point coordinate)
{
    if (location == MAIN) {
        Intersection &intersection = *idIntersectionMap.find(mainLists[coordinate.x][coordinate.y]);
        if (intersection.visited) {
            result.setClosed(true);
            return;
        }
        intersection.visited = true;
        result.addPoint(QPoint(intersection.p.x, intersection.p.y));
        QPair<Location, Point> Next = NextPoint(location, coordinate);
        Dfs(result, Next.first, Next.second);
    } else {
        Intersection &intersection = *idIntersectionMap.find(cutLists[coordinate.x][coordinate.y]);
        if (intersection.visited) {
            result.setClosed(true);
            return;
        }
        intersection.visited = true;
        result.addPoint(QPoint(intersection.p.x, intersection.p.y));
        QPair<Location, Point> Next = NextPoint(location, coordinate);
        Dfs(result, Next.first, Next.second);
    }
}

Polygon MyMath::Calculate(Polygon &mainPolygon)
{
    Polygon result;
    for (int mainId = 0; mainId < mainLists.size(); mainId++) {
        for (int mainPointId = 0; mainPointId < mainLists[mainId].size(); mainPointId++) {
            Intersection beginIntersection = *idIntersectionMap.find(mainLists[mainId][mainPointId]);
            if (beginIntersection.type == IN && !beginIntersection.visited) {
                Dfs(result, MAIN, Point(mainId, mainPointId));
            }
        }
    }

    if (result.hasNextPolygon()) {
        return result;
    } else {
        return mainPolygon;
    }
}

Polygon MyMath::polygonIntersection(Polygon &mainPolygon, Polygon &cutPolygon)
{
    // Rest containers
    resetContainers();

    // Init intersection lists
    initContainers(mainPolygon, cutPolygon);

    // Calculate the intersection of each pair of edges
    GetIntersections(mainPolygon, cutPolygon);

    // Sort intersections using distance to points and re-build intersection lists
    SortIntersections(mainPolygon, cutPolygon);

    // Calculate result polygon
    return Calculate(mainPolygon);
}

void MyMath::PrintAllPoint(Polygon &mainPolygon, Polygon &cutPolygon)
{
    for (int subPolygonId = 0; subPolygonId < mainPolygon.subPolygons.size(); subPolygonId++) {
        for (int subPointId = 0; subPointId < mainPolygon.subPolygons[subPolygonId].size(); subPointId++) {
            Point point = mainPolygon.subPolygons[subPolygonId][subPointId];
            printf("%d %d\n", point.x, point.y);
        }
    }
    for (int subPolygonId = 0; subPolygonId < cutPolygon.subPolygons.size(); subPolygonId++) {
        for (int subPointId = 0; subPointId < cutPolygon.subPolygons[subPolygonId].size(); subPointId++) {
            Point point = cutPolygon.subPolygons[subPolygonId][subPointId];
            printf("%d %d\n", point.x, point.y);
        }
    }
    for (int listId = 0; listId < intersectionLists.size(); listId++) {
        for (int intersectionId = 0; intersectionId < intersectionLists[intersectionId].size(); intersectionId++) {
            printf("%d %d\n", intersectionLists[listId][intersectionId].p.x, intersectionLists[listId][intersectionId].p.y);
        }
    }
}
