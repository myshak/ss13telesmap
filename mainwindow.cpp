#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calibrationdialog.h"

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    scene(new MapScene(this)),
    mx(0),
    my(0),
    cx(0),
    cy(0),
    selected(false),
    sx(0),
    sy(0)
{
    ui->setupUi(this);

    QSettings settings("maps.ini", QSettings::IniFormat);

    QActionGroup *action_group = new QActionGroup(this);

    QStringList map_entries = settings.childGroups();
    for(auto &map: map_entries) {
        settings.beginGroup(map);
        Map m;
        m.path = settings.value("path").toString();
        m.name = map;
        maps.append(m);
        settings.endGroup();

        QAction *action = new QAction(map,ui->menuMaps);
        action->setData(maps.size() - 1); // Index to the current map
        action->setCheckable(true);
        action->setActionGroup(action_group);
        ui->menuMaps->addAction(action);
    }

    if(maps.size() > 0) {
        currentMap = &(maps[0]);
        scene->setMap(&(maps[0]));
        ui->menuMaps->actions()[0]->setChecked(true);
        setWindowTitle(QString("%1 - %2").arg(QCoreApplication::applicationName()).arg(maps[0].name));
    }

    ui->graphicsView->setScene(scene);
    ui->graphicsView->calculateZoom();
    QApplication::setOverrideCursor( Qt::ArrowCursor );
    ui->graphicsView->show();

    ui->stackedWidget->hide();
    ui->tableWidget->setHorizontalHeaderLabels({"Name", "Map", "X", "Y"});
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    ui->tableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    ui->tableWidget->horizontalHeader()->setResizeMode(0, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Fixed);
    ui->tableWidget->horizontalHeader()->setResizeMode(3, QHeaderView::Fixed);

    ui->tableWidget->horizontalHeader()->resizeSection(2, 50);
    ui->tableWidget->horizontalHeader()->resizeSection(3, 50);

    QSettings bookmark_settings("bookmarks.ini", QSettings::IniFormat);

    int bookmark_size = bookmark_settings.beginReadArray("bookmark");
    for(int i=0; i < bookmark_size; i++) {
        bookmark_settings.setArrayIndex(i);
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(bookmark_settings.value("name").toString()));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(bookmark_settings.value("map").toString()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(bookmark_settings.value("x").toString()));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(bookmark_settings.value("y").toString()));
    }
    bookmark_settings.endArray();

    bg = new QButtonGroup(this);
    bg->setExclusive(true);
    bg->addButton(ui->button_favourites);
    bg->setId(ui->button_favourites, 0);
    bg->addButton(ui->button_usage);
    bg->setId(ui->button_usage, 1);
    QObject::connect(bg, SIGNAL(buttonClicked(int)), this, SLOT(toolbutton_pressed(int)));

    QObject::connect(ui->menuMaps, SIGNAL(triggered(QAction*)), this, SLOT(map_selected(QAction*)));
    QObject::connect(scene, SIGNAL(mousepressed(QMouseEvent*)), this, SLOT(pressed(QMouseEvent*)));
    QObject::connect(scene, SIGNAL(mousemoved(QMouseEvent*)), this, SLOT(moved(QMouseEvent*)));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::pressed(QMouseEvent *e)
{
    if(e->button() == Qt::MiddleButton) {
        ui->graphicsView->resetZoom();
    } else if(e->button() == Qt::RightButton) {


        selected = true;
        sx = 1+ e->x()/32;
        sy = 1+ e->y()/32;

        ui->label_ox->setText(QString("%1").arg(sx));
        ui->label_oy->setText(QString("%1").arg(sy));


        bool e_mx_ok = false;
        bool e_my_ok = false;
        ui->edit_mx->currentText().toInt(&e_mx_ok);
        ui->edit_my->currentText().toInt(&e_my_ok);

        if(e_mx_ok && e_my_ok) {
            float tx = (static_cast<float>(sx)+cx) / mx;
            float ty = (static_cast<float>(sy)+cy) / my;
            ui->label_tx->setText(QString("%1").arg(tx));
            ui->label_ty->setText(QString("%1").arg(ty));
        }
        e->accept();
    }
}

void MainWindow::moved(QMouseEvent *e)
{

    int ex = e->x();
    int ey = e->y();
    int x = 1+ex/32;
    int y = 1+ey/32;

    ui->statusBar->showMessage(QString("X: %1 / Y: %2").arg(x).arg(y));

}

MapScene::MapScene(QObject *parent)
    : QGraphicsScene(parent),
      currentMap(nullptr),
      map(nullptr)
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
    if(map) {
        this->removeItem(map);
        delete map;
    }

    QPixmap map_pix(currentMap->path);
    map = this->addPixmap(map_pix);
    map->setZValue(0);
    setSceneRect(0,0,map_pix.width(), map_pix.height());
    selectedSquare->hide();
    highlightedSquare->hide();
}

Map *MapScene::getMap() const
{
    return currentMap;
}

void MapScene::highligt(QPoint pos)
{
    highlightedSquare->setPos(pos);
    highlightedSquare->show();
}

void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pf(event->scenePos().x(), height() - event->scenePos().y());
    QMouseEvent ne(event->type(), pf.toPoint(), event->button(), event->buttons(), event->modifiers());
    emit mousepressed(&ne);

    if(event->button() == Qt::RightButton) {
        int x = event->scenePos().x()/32;
        int y = (event->scenePos().y() + 0)/32; // TODO: ?

        selectedSquare->setX(x*32);
        selectedSquare->setY(y*32 - 1); // TODO: ?
        selectedSquare->show();
    }

    QGraphicsScene::mousePressEvent(event);
}

void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pf(event->scenePos().x(), height() - event->scenePos().y());
    QMouseEvent ne(event->type(), pf.toPoint(), event->button(), event->buttons(), event->modifiers());
    emit mousemoved(&ne);

    int x = event->scenePos().x()/32;
    int y = (event->scenePos().y() + 0)/32; // TODO: ?
    int x_pos = x*32;
    int y_pos = y*32 - 1; // TODO: ?

    highligt(QPoint{x_pos, y_pos});

    QGraphicsScene::mouseMoveEvent(event);
}

void MainWindow::on_actionCalibrate_triggered()
{
    CalibrationDialog d(this);
    QObject::connect(&d, SIGNAL(calibrated(qreal,qreal,qreal,qreal)), this, SLOT(calibrated(qreal,qreal,qreal,qreal)));
    d.exec();
}

void MainWindow::toolbutton_pressed(int id)
{

    if(ui->stackedWidget->isVisible() && ui->stackedWidget->currentIndex() == id) {
        ui->stackedWidget->hide();
        bg->setExclusive(false);
        bg->checkedButton()->setChecked(false);
        bg->setExclusive(true);
    } else {
        ui->stackedWidget->setCurrentIndex(id);
        ui->stackedWidget->show();
    }
}


void MainWindow::calibrated(qreal mx, qreal my, qreal cx, qreal cy)
{
    if(mx == 1 || mx == 2) {
        ui->edit_mx->setCurrentIndex(mx);
    } else if (mx == 4) {
        ui->edit_mx->setCurrentIndex(3);
    }

    if(my == 1 || my == 2) {
        ui->edit_my->setCurrentIndex(my);
    } else if (mx == 4) {
        ui->edit_my->setCurrentIndex(3);
    }

    ui->edit_cx->setValue(cx);
    ui->edit_cy->setValue(cy);
    update_params();
}

void MainWindow::map_selected(QAction *a)
{
    for(QAction *i: ui->menuMaps->actions())
    {
        i->setChecked(false);
    }

    currentMap = &maps[a->data().toInt()];
    scene->setMap(&maps[a->data().toInt()]);

    a->setChecked(true);
    setWindowTitle(QString("%1 - %2").arg(QCoreApplication::applicationName()).arg(maps[a->data().toInt()].name));
}

void MainWindow::recalculate_manual()
{
    bool e_mx_ok = false;
    bool e_my_ok = false;
    ui->edit_mx->currentText().toInt(&e_mx_ok);
    ui->edit_my->currentText().toInt(&e_my_ok);

    if(e_mx_ok && e_my_ok) {
        qreal mox = ui->edit_mox->value();
        qreal moy = ui->edit_moy->value();
        float tx = (mox+cx) / mx;
        float ty = (moy+cy) / my;
        ui->label_mtx->setText(QString("%1").arg(tx));
        ui->label_mty->setText(QString("%1").arg(ty));
    }
}

void MainWindow::update_params()
{
    cx = ui->edit_cx->value();
    cy = ui->edit_cy->value();

    bool mx_ok = false;
    bool my_ok = false;
    mx = ui->edit_mx->currentText().toInt(&mx_ok);
    my = ui->edit_my->currentText().toInt(&my_ok);

    // Recalculate only with valid constants
    if(mx_ok && my_ok) {
        recalculate_manual();
        on_tableWidget_itemSelectionChanged();
    } else {
        ui->label_mtx->clear();
        ui->label_mty->clear();
        ui->label_btx->clear();
        ui->label_bty->clear();
    }

    ui->label_tx->clear();
    ui->label_ty->clear();
}


Map::Map(QString path)
    : path(path)
{
}

void MainWindow::on_pushButton_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
}

void MainWindow::on_pushButton_3_clicked()
{
    QSettings bookmark_settings("bookmarks.ini", QSettings::IniFormat);
    bookmark_settings.clear();

    bookmark_settings.beginWriteArray("bookmark");
    for(int i=0; i < ui->tableWidget->rowCount(); i++) {
        bookmark_settings.setArrayIndex(i);
        bookmark_settings.setValue("name", ui->tableWidget->item(i, 0)->text());
        bookmark_settings.setValue("map", ui->tableWidget->item(i, 1)->text());
        bookmark_settings.setValue("x", ui->tableWidget->item(i, 2)->text());
        bookmark_settings.setValue("y", ui->tableWidget->item(i, 3)->text());
    }
    bookmark_settings.endArray();

}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    int row = ui->tableWidget->currentRow();
    if(row == -1) return;

    if (!ui->tableWidget->item(row, 2) ||
        !ui->tableWidget->item(row, 3)) return;

    int bx = ui->tableWidget->item(row, 2)->text().toInt();
    int by = ui->tableWidget->item(row, 3)->text().toInt();

    if(currentMap->name == ui->tableWidget->item(row,1)->text()) {
        int x_pos = bx*32;
        int y_pos = scene->height() - by*32;
        ui->graphicsView->centerOn(x_pos+16, y_pos+16);

        scene->highligt(QPoint{x_pos -32, y_pos});
    }

    if(mx != 0 &&
       my != 0) {
        float tx = (bx+cx) / mx;
        float ty = (by+cy) / my;

        ui->label_bname->setText(ui->tableWidget->item(row, 0)->text());
        ui->label_bmap->setText(ui->tableWidget->item(row, 1)->text());
        ui->label_box->setText(QString("%1").arg(bx));
        ui->label_boy->setText(QString("%1").arg(by));
        ui->label_btx->setText(QString("%1").arg(tx));
        ui->label_bty->setText(QString("%1").arg(ty));
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(selected) {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem("Bookmark"));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem(currentMap->name));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString("%1").arg(sx)));
        ui->tableWidget->setItem(row, 3, new QTableWidgetItem(QString("%1").arg(sy)));
    }
}




void MainWindow::on_actionAbout_SS13_Telescience_Manager_triggered()
{
    QMessageBox::about(this, tr("About Telescience Manager"), trUtf8("Telescience Manager\n© 2013-2014 by mysha (mysha@mysha.cu.cc)\n"
                                                            "\n"
                                                            "Donations in bitcoins or goon membership are appreciated.\n\n"
                                                            "BTC donation address: %0").arg("1Gzk3F4C4FiMVjTHCCkRuRwqZoCKujtBXd"));
}
