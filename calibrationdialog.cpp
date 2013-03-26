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
    qreal mx, my, cx, cy;

    mx = ui->ox2->value() - ui->ox1->value();
    my = ui->oy2->value() - ui->oy1->value();

    cx = ui->ox1->value() - mx * ui->ix1->value();
    cy = ui->oy1->value() - my * ui->iy1->value();

    emit calibrated(mx,my,cx,cy);

    QDialog::accept();
}
