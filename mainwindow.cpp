#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(this, SIGNAL(addPointSignal(QMouseEvent*)), this, SLOT(addPoint(QMouseEvent*)));
    connect(this, SIGNAL(closedSignal()), this, SLOT(closed()));
    connect(this->ui->mainPolyButton, SIGNAL(pressed()), this, SLOT(setMainPolygonMode()));
    connect(this->ui->cutPolyButton, SIGNAL(pressed()), this, SLOT(setCutPolygonMode()));
    connect(this->ui->clearButton, SIGNAL(pressed()), this, SLOT(resetPolygon()));
    connect(this->ui->cutButton, SIGNAL(pressed()), this, SLOT(Calculate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit addPointSignal(event);
    } else if (event->button() == Qt::RightButton) {
        emit closedSignal();
    }
}

void MainWindow::setMainPolygonMode()
{
    this->editMode = MAIN_POLYGON;
}

void MainWindow::setCutPolygonMode()
{
    this->editMode = CUT_POLYGON;
}

void MainWindow::resetPolygon()
{
    this->mainPolygon.reset();
    this->cutPolygon.reset();
    this->resultPolygon.reset();
}

void MainWindow::addPoint(QMouseEvent *event)
{
    switch (editMode) {
        case MAIN_POLYGON:
            this->mainPolygon.addPoint(event->pos());
            break;
        case CUT_POLYGON:
            this->cutPolygon.addPoint(event->pos());
            break;
    }
}

void MainWindow::closed()
{
    switch (editMode) {
        case MAIN_POLYGON:
            this->mainPolygon.setClosed(true);
            break;
        case CUT_POLYGON:
            this->cutPolygon.setClosed(true);
            break;
    }
}

void MainWindow::Calculate()
{
    MyMath math;
    if (mainPolygon.isClosed() && cutPolygon.isClosed()) {
        resultPolygon = math.polygonIntersection(mainPolygon, cutPolygon);
    }
    //mainPolygon.reset();
    //cutPolygon.reset();
}

void MainWindow::paintPolygon(Polygon &polygon, QColor color, FillType fill)
{
    QPainter painter(this);
    QPen pointPen(color);
    pointPen.setWidth(3);
    painter.setPen(pointPen);

    for (int subPolygonId = 0; subPolygonId < polygon.subPolygons.size(); subPolygonId++) {
        QVector<QPoint> &subPolygon = polygon.subPolygons[subPolygonId];
        if (subPolygon.size() == 1) {
            painter.drawPoint(subPolygon[0]);
        } else {
            QPainterPath path;
            path.moveTo(subPolygon[0]);
            for (int subPointId = 1; subPointId < subPolygon.size(); subPointId++) {
                path.lineTo(subPolygon[subPointId]);
            }
            if (subPolygonId < polygon.subPolygons.size() - 1 || polygon.isClosed()) {
                path.lineTo(subPolygon[0]);
            }
            painter.drawPath(path);
            if (fill == ALL) {
                painter.fillPath(path, color);
            } else if (fill == INSIDE && isCut && subPolygonId > 0 && polygon.isClosed()) {
                painter.fillPath(path, QColor(240, 240, 240));
            }
        }
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    paintPolygon(resultPolygon, Qt::yellow, ALL);
    if (resultPolygon.hasNextPolygon()) {
        isCut = true;
    } else {
        isCut = false;
    }
    paintPolygon(mainPolygon, Qt::red, INSIDE);
    paintPolygon(cutPolygon, Qt::blue, INSIDE);

    update();
}
