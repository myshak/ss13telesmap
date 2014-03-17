#include "calibrationdialog.h"
#include "ui_calibrationdialog.h"

CalibrationDialog::CalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalibrationDialog)
{
    ui->setupUi(this);
}

CalibrationDialog::~CalibrationDialog()
{
    delete ui;
}

void CalibrationDialog::accept()
{
        // FIXME: orig 50,50 -> 49,49 is buggy, fix
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

    QDialog::accept();
}
