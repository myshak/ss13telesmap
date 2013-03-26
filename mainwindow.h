#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>

namespace Ui {
class MainWindow;
}

class Map {
public:
    Map() = default;
    Map(QString path, QPoint offsetTopLeft, QPoint offsetBottomRight, QPoint tileOffset);

    QString path;
    QString name;

    int mapTileOffsetX;
    int mapTileOffsetY;

    int mapPixelOffsetLeftX;  // From left edge
    int mapPixelOffsetTopY; // From top edge
    int mapPixelOffsetBottomY; // From bottom edge
};

class MapScene : public QGraphicsScene
{
    Q_OBJECT

public:
    MapScene(QObject * parent = 0);
    void setMap(Map *m);
    Map *getMap() const;

signals:
     void mousepressed(QMouseEvent*);
     void mousemoved(QMouseEvent*);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
private:
    QGraphicsRectItem *selectedSquare;
    QGraphicsRectItem *highlightedSquare;
    Map *currentMap;
    QGraphicsPixmapItem *map;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void pressed(QMouseEvent * e);
    void moved(QMouseEvent * e);
    void calibrated(qreal mx, qreal my, qreal cx, qreal cy);
    void map_selected(QAction *a);
    
private slots:
    void on_actionCalibrate_triggered();

private:
    Ui::MainWindow *ui;
    MapScene *scene;
    Map *currentMap;

    QVector<Map> maps;
};
#endif // MAINWINDOW_H
