#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "calibrationdialog.h"

const int mapTileOffsetX = 26;
const int mapTileOffsetY = 31;

const int mapPixelOffsetLeftX = 32 - 1;  // From left edge
const int mapPixelOffsetBottomY = 32 - 17; // From bottom edge
const int mapPixelOffsetTopY = 9; // From top edge

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MapScene* scene = new MapScene;

    QPixmap map("./map.gif");
    QGraphicsPixmapItem* p = scene->addPixmap(map);
    p->setZValue(0);

    ui->graphicsView->setScene(scene);
    QApplication::setOverrideCursor( Qt::ArrowCursor );
    ui->graphicsView->show();

    QObject::connect(scene, SIGNAL(mousepressed(QMouseEvent*)), this, SLOT(pressed(QMouseEvent*)));
    QObject::connect(scene, SIGNAL(mousemoved(QMouseEvent*)), this, SLOT(moved(QMouseEvent*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pressed(QMouseEvent *e)
{
    if(e->button() != Qt::RightButton) return;

    if(ui->edit_mx->value() != 0 &&
       ui->edit_my->value() != 0) {
        int x = e->x()/32 + mapTileOffsetX;
        int y = e->y()/32 + mapTileOffsetY;
        qreal mx,my,cx,cy;
        mx = ui->edit_mx->value();
        my = ui->edit_my->value();
        cx = ui->edit_cx->value();
        cy = ui->edit_cy->value();
        float tx = (static_cast<float>(x)-cx) / mx;
        float ty = (static_cast<float>(y)-cy) / my;
        ui->label_ox->setText(QString("%1").arg(x));
        ui->label_oy->setText(QString("%1").arg(y));
        ui->label_tx->setText(QString("%1").arg(tx));
        ui->label_ty->setText(QString("%1").arg(ty));
    }
    e->accept();
}

void MainWindow::moved(QMouseEvent *e)
{
    int x = e->x()/32 + mapTileOffsetX;
    int y = e->y()/32 + mapTileOffsetY;

    ui->statusBar->showMessage(QString("X: %1 / Y: %2").arg(x).arg(y));

}

MapScene::MapScene(QObject *parent)
    : QGraphicsScene(parent)
{
    QBrush b(QColor(255,0,0,64));
    QPen p;
    p.setColor(QColor(255,0,0,255));
    p.setWidth(2);
    p.setJoinStyle(Qt::RoundJoin);

    selectedSquare = this->addRect(0,0,32,32,p,b);
    selectedSquare->setZValue(101);

    b.setColor(QColor(0,255,0,64));
    p.setColor(QColor(0,64,0,255));

    highlightedSquare = this->addRect(0,0,32,32,p,b);
    highlightedSquare->setZValue(100);
}

void MapScene::setMap(Map *m)
{
    currentMap = m;
}

Map *MapScene::getMap()
{
    return currentMap;
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pf(event->scenePos().x() - mapPixelOffsetLeftX, height() - event->scenePos().y() - mapPixelOffsetBottomY);
    QMouseEvent ne(event->type(), pf.toPoint(), event->button(), event->buttons(), event->modifiers());
    emit mousepressed(&ne);

    int x = (event->scenePos().x())/32;
    int y = (event->scenePos().y() + 16 + mapPixelOffsetTopY)/32;

    selectedSquare->setX(x*32);
    selectedSquare->setY(y*32 - (32 - mapPixelOffsetTopY) - 1);
    selectedSquare->show();

    QGraphicsScene::mousePressEvent(event);
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pf(event->scenePos().x() - mapPixelOffsetLeftX, height() - event->scenePos().y() - mapPixelOffsetBottomY);
    QMouseEvent ne(event->type(), pf.toPoint(), event->button(), event->buttons(), event->modifiers());
    emit mousemoved(&ne);

    int x = (event->scenePos().x())/32;
    int y = (event->scenePos().y() + 16 + mapPixelOffsetTopY)/32;

    highlightedSquare->setX(x*32 );
    highlightedSquare->setY(y*32 - (32 - mapPixelOffsetTopY) - 1);
    highlightedSquare->show();



    QGraphicsScene::mouseMoveEvent(event);
}

void MainWindow::on_actionCalibrate_triggered()
{
    CalibrationDialog d(this);
    QObject::connect(&d, SIGNAL(calibrated(qreal,qreal,qreal,qreal)), this, SLOT(calibrated(qreal,qreal,qreal,qreal)));
    d.exec();
}


void MainWindow::calibrated(qreal mx, qreal my, qreal cx, qreal cy)
{
    ui->edit_mx->setValue(mx);
    ui->edit_my->setValue(my);
    ui->edit_cx->setValue(cx);
    ui->edit_cy->setValue(cy);
}


Map::Map(QString path, QPoint offsetTopLeft, QPoint offsetBottomRight, QPoint tileOffset)
    : path(path),
      mapTileOffsetX(tileOffset.x()),
      mapTileOffsetY(tileOffset.y()),
      mapPixelOffsetLeftX(offsetTopLeft.x()),
      mapPixelOffsetTopY(offsetTopLeft.y()),
      mapPixelOffsetBottomY(offsetBottomRight.y())
{
}

