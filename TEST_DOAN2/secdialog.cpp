#include "secdialog.h"
#include "ui_secdialog.h"


#include<QTimer>
#include<QDateTime>

SecDialog::SecDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SecDialog)
{
    ui->setupUi(this);
    QTimer *timercl= new QTimer(this);
    connect(timercl,SIGNAL(timeout()),this,SLOT(showTime()));
    timercl->start();

    buffer = "" ;
    code  = "\n" ;
    code2 = "%" ;
    codeSize = code.length();

    serialPort.setPortName("COM7");
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    serialPort.open(QIODevice::ReadWrite);


    ui->plot_dapung->setAutoAddPlottableToLegend(false);
    ui->plot_dapung->xAxis->setLabel("Time");
    ui->plot_dapung->yAxis->setLabel("Value");
    ui->plot_dapung->addGraph();
    ui->plot_dapung->graph(0)->setName("Data");
    ui->plot_dapung->graph(0)->setPen(QPen(Qt::red));
    ui->plot_dapung->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_dapung->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 5));
    ui->plot_dapung->xAxis->setRange(0, PLOT_RANGE, Qt::AlignLeft);
    ui->plot_dapung->yAxis->setRange(0, 550);
    ui->plot_dapung->replot();

    ui->plot_saiso->setAutoAddPlottableToLegend(false);
    ui->plot_saiso->xAxis->setLabel("Time");
    ui->plot_saiso->yAxis->setLabel("Value");
    ui->plot_saiso->addGraph();
    ui->plot_saiso->graph(0)->setName("SaiSo");
    ui->plot_saiso->graph(0)->setPen(QPen(Qt::red));
    ui->plot_saiso->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_saiso->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 5));
    ui->plot_saiso->xAxis->setRange(0, PLOT_RANGE, Qt::AlignLeft);
    ui->plot_saiso->yAxis->setRange(0, 550);
    ui->plot_saiso->replot();

    serialPort.write("d");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));
    timer->start(TIME_BETWEEN_FRAMES_MS);

    // Khởi tạo biến startTime là thời gian bắt đầu hiển thị đồ thị (lấy ra giá trị hiện tại và trừ đi DISPLAY_TIME)


    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

}
void SecDialog::showTime()
{
    QTime timeclk = QTime::currentTime();
    QString time_text=timeclk.toString("hh : mm : ss");
    ui->digital_clock->setText(time_text);
}
SecDialog::~SecDialog()
{
    delete ui;
}

void SecDialog::receiveMessage()
{
    QByteArray dataBA = serialPort.readAll();
    QString data(dataBA);
    buffer.append(data);
    int index = buffer.indexOf(code);
    //int index2= buffer.indexOf(code2);

    if (index != -1)
    {
        QString message = buffer.mid(0, index);
        //QString message2 = buffer.mid(0, index2);

        buffer.remove(0, index + codeSize);

        // Process the received message
        //double value = message.toDouble();
       // double value2 = message2.toDouble();
        QStringList values = message.split("%");
                if (values.size() != 2) {
                    // Error handling for invalid data
                    return;
                }

                // Convert the values to doubles
        double value1 = values[0].toDouble();
        double value2 = values[1].toDouble();

        if(isRealTimeEnabled)
        {
             // Add the received data to the plot
            //static double time = 0.0; // Initialize time variable

              if (time <= PLOT_RANGE)
              {
                 ui->plot_dapung->graph(0)->addData(time, value1);
                 ui->plot_dapung->replot();
                 ui->plot_saiso->graph(0)->addData(time, value2);
                 ui->plot_saiso->replot();
                 if(time==PLOT_RANGE)
                 {
                     serialPort.write("d");
                     isRealTimeEnabled = false;

                 }else
                 {
                     time += 0.1; // Increase time variable by 0.1 second for each data point
                 }
              }
        }
  }
}

void SecDialog::on_load_slider_valueChanged(int value)
{
    ui->label_slider->setText(QString::number(value));
}


void SecDialog::on_send_btn_clicked()
{
    // Lấy giá trị từ các edit text
    float Kp    = ui->KpLineEdit->text().toFloat();
    float Kd    = ui->KdLineEdit->text().toFloat();
    float Ki    = ui->KiLineEdit->text().toFloat();
    float Ag    = ui->AngleLineEdit->text().toFloat();
    float load  = ui->load_slider->value();
    // Gửi chuỗi chứa giá trị Kp, Kd, Ki qua USART
    QString data = QString("Kp:%1,Kd:%2,Ki:%3,Ag:%4,Load:%5").arg(Kp).arg(Kd).arg(Ki).arg(Ag).arg(load);
    QByteArray byteArray = data.toUtf8();
    serialPort.write(byteArray);
}


void SecDialog::on_start_btn_clicked()
{
    isRealTimeEnabled = true;
    //timer->start(TIME_BETWEEN_FRAMES_MS);
    serialPort.write("o");
}


void SecDialog::on_stop_btn_clicked()
{
    isRealTimeEnabled = false;
    serialPort.write("d");
}


void SecDialog::on_reset_btn_clicked()
{
    clearData(); // Xóa dữ liệu đồ thị

    isRealTimeEnabled = false; // Đặt lại biến isRealTimeEnabled
    time = 0.0; // Khởi đầu lại biến time
    buffer = "";
    code = "\n";
    code2 = "%" ;
    codeSize = code.length();

    serialPort.setPortName("COM7");
    serialPort.setBaudRate(QSerialPort::Baud115200);
    serialPort.setDataBits(QSerialPort::Data8);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    serialPort.open(QIODevice::ReadWrite);

    ui->plot_dapung->setAutoAddPlottableToLegend(false);
    ui->plot_dapung->xAxis->setLabel("Time");
    ui->plot_dapung->yAxis->setLabel("Value");
    ui->plot_dapung->addGraph();
    ui->plot_dapung->graph(0)->setName("Data");
    ui->plot_dapung->graph(0)->setPen(QPen(Qt::red));
    ui->plot_dapung->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_dapung->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 5));
    ui->plot_dapung->xAxis->setRange(0, PLOT_RANGE, Qt::AlignLeft);
    ui->plot_dapung->yAxis->setRange(0, 550);
    ui->plot_dapung->replot();

    ui->plot_saiso->setAutoAddPlottableToLegend(false);
    ui->plot_saiso->xAxis->setLabel("Time");
    ui->plot_saiso->yAxis->setLabel("Value");
    ui->plot_saiso->addGraph();
    ui->plot_saiso->graph(0)->setName("SaiSo");
    ui->plot_saiso->graph(0)->setPen(QPen(Qt::red));
    ui->plot_saiso->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->plot_saiso->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossCircle, 5));
    ui->plot_saiso->xAxis->setRange(0, PLOT_RANGE, Qt::AlignLeft);
    ui->plot_saiso->yAxis->setRange(0, 550);
    ui->plot_saiso->replot();

    serialPort.write("d");
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTick()));
    timer->start(TIME_BETWEEN_FRAMES_MS);

    // Khởi tạo biến startTime là thời gian bắt đầu hiển thị đồ thị (lấy ra giá trị hiện tại và trừ đi DISPLAY_TIME)

    connect(&serialPort, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
}

void SecDialog::clearData()
{
    ui->plot_dapung->clearPlottables();
    ui->plot_dapung->replot();
    ui->plot_saiso->clearPlottables();
    ui->plot_saiso->replot();
}

