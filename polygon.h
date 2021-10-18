#ifndef POLYGON_H
#define POLYGON_H

#include <QVector>
#include <QPoint>

class Polygon
{
private:
    int polygonCnt = 0;

public:
    Polygon();
    ~Polygon();

    void addPoint(QPoint point);
    void setClosed(bool isClosed);
    bool isClosed();
    void reset();

    bool hasNextPolygon();
    QVector<QPoint>& nextPolygon();
    void resetPolygonCnt();

    bool closed = true;
    QVector<QVector<QPoint> > subPolygons;
};

#endif // POLYGON_H
