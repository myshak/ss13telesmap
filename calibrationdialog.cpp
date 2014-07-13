#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"

CalibrationDialog::CalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);

    ui->x_max_group->setId(ui->max_x_1, 1);
    ui->x_max_group->setId(ui->max_x_2, 2);
    ui->x_max_group->setId(ui->max_x_4, 4);

    ui->y_max_group->setId(ui->max_y_1, 1);
    ui->y_max_group->setId(ui->max_y_2, 2);
    ui->y_max_group->setId(ui->max_y_4, 4);
}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}

void CalibrationDialog::calibrate_gps()
{
    qreal ix1 = ui->ix1->value();
    qreal ix2 = ui->ix2->value();

    qreal iy1 = ui->iy1->value();
    qreal iy2 = ui->iy2->value();

    qreal ox1 = ui->ox1->value();
    qreal ox2 = ui->ox2->value();

    qreal oy1 = ui->oy1->value();
    qreal oy2 = ui->oy2->value();

    qreal mx, my, cx, cy;

    if(ix1 < ix2) {
        mx = ox2 - ox1;
    } else {
        mx = ox1 - ox2;
    }

    if(iy1 < iy2) {
        my = oy2 - oy1;
    } else {
        my = oy1 - oy2;
    }

    cx = ox1 - mx * ix1;
    cy = oy1 - my * iy1;

    emit calibrated(mx,my,-cx,-cy);
}

void CalibrationDialog::calibrate_extrapolation()
{
    QAbstractButton* b_maxx = ui->x_max_group->checkedButton();
    QAbstractButton* b_maxy = ui->y_max_group->checkedButton();

    if(!b_maxx || !b_maxy) {
        // 'Check a box' message?
        return;
    }

    qreal maxx = b_maxx->text().toDouble();
    qreal maxy = b_maxy->text().toDouble();

    int mx = ui->x_max_group->checkedId();
    int my = ui->y_max_group->checkedId();

    int cx = maxx*mx - ui->map_size_x->value();
    int cy = maxy*my - ui->map_size_y->value();

    emit calibrated(mx,my,cx,cy);
}

void CalibrationDialog::update_max_values()
{
    qreal minx = ui->edit_minx->value();
    qreal miny = ui->edit_miny->value();

    float map_x = static_cast<float>(ui->map_size_x->value() - 1);
    float map_y = static_cast<float>(ui->map_size_y->value() - 1);

    ui->max_x_1->setText(QString::number(map_x + minx));
    ui->max_x_2->setText(QString::number(map_x/2.0 + minx));
    ui->max_x_4->setText(QString::number(map_x/4.0 + minx));

    ui->max_y_1->setText(QString::number(map_y + miny));
    ui->max_y_2->setText(QString::number(map_y/2.0 + miny));
    ui->max_y_4->setText(QString::number(map_y/4.0 + miny));
}

void CalibrationDialog::accept()
{
    if(ui->tabWidget->currentIndex() == 0) {
        calibrate_gps();
    } else if(ui->tabWidget->currentIndex() == 1) {
        calibrate_extrapolation();
    }

    QDialog::accept();
}
