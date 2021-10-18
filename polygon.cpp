#include "polygon.h"

Polygon::Polygon()
{

}

Polygon::~Polygon()
{

}

void Polygon::addPoint(QPoint point)
{
    if (this->closed) {
        QVector<QPoint> newPoly;
        newPoly.push_back(point);
        this->subPolygons.push_back(newPoly);
        this->closed = false;
    } else {
        int cnt = this->subPolygons.size() - 1;
        this->subPolygons[cnt].push_back(point);
    }
}


void Polygon::setClosed(bool isClosed)
{
    this->closed = isClosed;
}

bool Polygon::isClosed()
{
    return closed;
}

void Polygon::reset()
{
    this->closed = true;
    this->subPolygons.clear();
}

bool Polygon::hasNextPolygon()
{
    return polygonCnt < subPolygons.size();
}

QVector<QPoint>& Polygon::nextPolygon()
{
    return subPolygons[polygonCnt++];
}

void Polygon::resetPolygonCnt()
{
    this->polygonCnt = 0;
}
