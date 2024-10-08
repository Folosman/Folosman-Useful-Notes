#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QMessageBox>
#include <QDataStream>
#include <QtEndian>

#include <QDebug>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialport(new QSerialPort(this))
{
    ui->setupUi(this);

    timer = new QTimer(this);

    serialport = new QSerialPort(this);
    serialport->setPortName("XXXXXXXXXX"); 
    serialport->setBaudRate(QSerialPort::Baud115200);
    serialport->setDataBits(QSerialPort::Data8);
    serialport->setParity(QSerialPort::NoParity);
    serialport->setStopBits(QSerialPort::OneStop);
    serialport->setFlowControl(QSerialPort::NoFlowControl);
    if (serialport->open(QIODevice::ReadWrite))
    {
        ui->label->setText("Port Open!");
    }
    else
    {
        ui->label->setText("Port close!");
    }
    connect(serialport, &QSerialPort::readyRead, this, &MainWindow::serialPortReader);

    // connect(timer, &QTimer::timeout, this, &MainWindow::serialPortWritter);

    // timer->start(3000);
    // serialport->write(QString("\n333").toStdString().c_str());
    // uchar DDD[1] {0b01010111};
}

MainWindow::~MainWindow()
{
    serialport->close();
    delete ui;
}

QString MainWindow::serialPortReader()
{
    const QByteArray data = serialport->readAll();

    qDebug() << data;

    // qDebug() << data.toStdString().c_str();
    ui->m_labelByte->setText(QString(data));
    QByteArray beginData = data.mid(2,1);
    qDebug() << beginData.toInt();

    // Переделать в switch

    if(beginData.toInt() == ECHO_PACKAGE)
    {
        serialport->waitForBytesWritten(30);
    }
    else
    {
        qDebug() << QString("UNKNOW PACKAGE").toStdString().c_str();
    }

    return "";
}




        /*      В мастер        */
void MainWindow::serialPortWritter(const QByteArray &begin)
{
    // QByteArray bytes;
    // Тестовая передача данных

    QByteArray data;

    // data.append((char*)marker);
    uchar start[2] = {0xFF, 0xFF};
    data.append((char*)start);
    data.append(begin);
    data.append((char*)endMarker);

    quint16 trash = trashCrc(data);
    data.append(trash);

    serialport->write(data);
}
