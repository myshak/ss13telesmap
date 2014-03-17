#ifndef CALIBRATIONDIALOG_H
#define CALIBRATIONDIALOG_H

#include <QDialog>

namespace Ui {
class CalibrationDialog;
}

class CalibrationDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit CalibrationDialog(QWidget *parent = 0);
    ~CalibrationDialog();

signals:
    void calibrated(qreal mx, qreal my, qreal cx, qreal cy);

protected :
    void accept();

public slots:
    void update_max_values();

private:
    Ui::CalibrationDialog *ui;
    void calibrate_extrapolation();
    void calibrate_gps();
};

#endif // CALIBRATIONDIALOG_H
