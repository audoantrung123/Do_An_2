#ifndef SECDIALOG_H
#define SECDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include  "qcustomplot.h"

#define TIME_BETWEEN_FRAMES_MS  100
#define PLOT_RANGE 10

namespace Ui {
class SecDialog;
}

class SecDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SecDialog(QWidget *parent = nullptr);
    ~SecDialog();

private slots:
    void receiveMessage();

    void showTime();

    void on_load_slider_valueChanged(int value);

    void on_send_btn_clicked();

    void on_start_btn_clicked();

    void on_stop_btn_clicked();

    void on_reset_btn_clicked();

    void clearData();

private:
    Ui::SecDialog *ui; 
    QCustomPlot *graph;
    QSerialPort serialPort;
    QSerialPortInfo info;
    QString buffer;
    QString code;
    QString code2;
    QTimer *timer;
    int codeSize;
    bool isRealTimeEnabled = false;
    double time = 0.0;
    int giatridat=100;

};

#endif // SECDIALOG_H
