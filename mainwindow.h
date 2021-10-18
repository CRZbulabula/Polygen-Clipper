#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QEvent>
#include <QMouseEvent>

#include "polygon.h"
#include "myMath.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum PolygonType {
  MAIN_POLYGON, CUT_POLYGON
};

enum FillType {
    ALL, INSIDE, NONE
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

private:
    Ui::MainWindow *ui;
    PolygonType editMode = MAIN_POLYGON;
    Polygon mainPolygon, cutPolygon, resultPolygon;
    bool isCut = false;

    void paintPolygon(Polygon &polygon, QColor color, FillType fillType);

signals:
    void addPointSignal(QMouseEvent *event);
    void closedSignal();

public slots:
    void setMainPolygonMode();
    void setCutPolygonMode();
    void resetPolygon();
    void addPoint(QMouseEvent *);
    void closed();
    void Calculate();

};
#endif // MAINWINDOW_H
