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
        // Check a box message?
        return;
    }

    qreal maxx = b_maxx->text().toDouble();
    qreal maxy = b_maxy->text().toDouble();

    int mx = ui->x_max_group->checkedId();
    int my = ui->x_max_group->checkedId();

    int cx = maxx*mx - 200;
    int cy = maxy*my - 200;

    emit calibrated(mx,my,cx,cy);
}

void CalibrationDialog::update_max_values()
{
    qreal minx = ui->edit_minx->value();
    qreal miny = ui->edit_miny->value();

    ui->max_x_1->setText(QString::number(199.0 + minx));
    ui->max_x_2->setText(QString::number(199.0/2 + minx));
    ui->max_x_4->setText(QString::number(199.0/4 + minx));

    ui->max_y_1->setText(QString::number(199.0 + miny));
    ui->max_y_2->setText(QString::number(199.0/2 + miny));
    ui->max_y_4->setText(QString::number(199.0/4 + miny));
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
