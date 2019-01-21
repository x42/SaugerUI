#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QMutex>

#include "c-miio/miio.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButtonStart_clicked();

    void on_pushButtonPause_clicked();

    void on_pushButtonHome_clicked();

    void on_pushButtonFind_clicked();

    void on_pushButtonRstSide_clicked();

    void on_pushButtonRstMain_clicked();

    void on_pushButtonRstFilter_clicked();

    void on_pushButtonRstSensor_clicked();

    void on_sliderFanSpeed_valueChanged(int value);

    void on_tme_now_clicked();

    void update_device_info ();

private:

    Ui::MainWindow *ui;
    QTimer _timer;
    QMutex _lock;

    json_value* locked_miio_cmd (const char* cmd, const char* opt);
    void reset_consumable (const char*, const char*);
    void check_ok (json_value*);
};

#endif // MAINWINDOW_H
