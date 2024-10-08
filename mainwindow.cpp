#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QMessageBox>
#include <QDataStream>
#include <QtEndian>

#include <QDebug>
#include <iostream>

// const QString ECHO_PACKAGE = "0x00";
// const uchar CARRIER_FREQUENCY_PACKAGE = 0x01;
// const uchar TELEMETRY_MODE_PACKAGE = 0x02;
// const uchar SPECTRUM_MODE_PACKAGE = 0x03;
// const uchar RECEIVER_STATE_PACKAGE = 0x04;
// const uchar TELEMETRY_PACKAGE = 0x10;
// const uchar SPECTRUM_PACKAGE = 0x11;
// const uchar UNKNOWN_PACKAGE = 0xFF;


const char ECHO_PACKAGE(0x00);
const char CARRIER_FREQUENCY_PACKAGE(0x01);
const char TELEMETRY_MODE_PACKAGE(0x02);
// const char SPECTRUM_MODE_PACKAGE(0x03);
const char RECEIVER_STATE_PACKAGE(0x04);
const char TELEMETRY_PACKAGE(0x10);
const char SPECTRUM_PACKAGE(0x11);
// const char UNKNOWN_PACKAGE(0xFF);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , serialport(new QSerialPort(this))
{
    ui->setupUi(this);

    timer = new QTimer(this);

    serialport = new QSerialPort(this);
    serialport->setPortName("/dev/ttyM0"); ///dev/ttyUSB0
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
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::pushBtn);

    // connect(timer, &QTimer::timeout, this, &MainWindow::serialPortWritter);

    // timer->start(3000);
    // serialport->write(QString("\n333").toStdString().c_str());

    uchar DDD[21] {0x2A, 0xD0, 0xCF, 0xFA, 0xE1, 0xFE, 0x1E, 0x57, 0xF5, 0xFD, 0x76, 0xB9, 0x0E, 0x1d, 0xEF, 0x24, 0xE0, 0x99, 0x9D, 0x00, 0x00};
    // uchar DDD[1] {0b01010111};
    qDebug() << DDD;
    QByteArray encoded;
    encoded.append((char*)(DDD));

    qDebug() << encoded;
    decoder.decode(encoded);
    ui->label_3->setText(MainWindow::serialPortReader());

    ui->label_4->setText(calculationOutput.latitudeProcessing(111.919033, 'S'));
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
        // std::cout << "\n" << beginData.data();
        // std::cout << "\n" << ECHO_PACKAGE.data();
        // std::cout << "\n" << data.mid(4).data();

        QMessageBox::critical(this, QString("SUS"), QString("SUS in beginner symbols!"));
        // serialport->waitForReadyRead(30);
        // serialport->write((QString("0x00") + (QString("11111"))).toStdString().c_str());
        // serialport->write(QString(ECHO_PACKAGE).toStdString().c_str());
        serialport->waitForBytesWritten(30);

        // return data;

        qDebug() << data;

    }

    else if(beginData.toInt() == CARRIER_FREQUENCY_PACKAGE)
    {
        portFreqInsert = data.mid(4);
        std::cout << portFreqInsert.data() << "\n";
    }

    else if(beginData.toInt() == TELEMETRY_MODE_PACKAGE)
    {
        portTelemetryEnable = data.mid(4);
    }

    else if(beginData.toInt() == CARRIER_FREQUENCY_PACKAGE)
    {
        portSpectreEnable = data.mid(4);
    }

    else if(beginData.toInt() == RECEIVER_STATE_PACKAGE)
    {
        receiverValue = data.mid(4);
        // portFreq;//unsigned int
        // portRegim;//bool
        // portSpectre;//bool
        // portRssi;//int
        // portVoltageUsb;//int
        // portVoltageMc;//int
        // portVoltageRu;//int
        // portVoltageOu;//int
        // portTemperature;//int
    }


    // timer
    else if(beginData.toInt() == TELEMETRY_PACKAGE)
    {
        telemetryValue = data.mid(4);
    }
    //timer
    else if(beginData.toInt() == SPECTRUM_PACKAGE)
    {
        spectrumValue = data.mid(4);
    }

    else
    {
        qDebug() << QString("UNKNOW PACKAGE").toStdString().c_str();
        // qDebug() << data;
    }


        // Обязательно
    // switch (*beginData) {
    // case '0x01':
    //     std::cout << "SAA";
    //     break;
    // default:
    //     break;
    // }
    return "";
}




        /*      В мастер        */
void MainWindow::serialPortWritter(const QByteArray &begin)
{
    // QByteArray bytes;
    // Тестовая передача данных

    QByteArray data;

    // data.append((char*)marker);
    uchar start[2] = {0x1B, 0x80};
    data.append((char*)start);
    data.append(begin);
    data.append((char*)endMarker);



    quint16 trash = trashCrc(data);
    data.append(trash);



    // qDebug() << trash << QString::number(trash, 16);
    qDebug() << data.toHex() << "!!!!!!!!!!";
    serialport->write(data);
}


//Костыль
bool MainWindow::ReadToWritte()
{
    return true;
}



void MainWindow::pushBtn()
{
    // uint16_t data = 3333;
    // char c [5] = {0x1B, 0x08, 0x11, 0x1B};
    // QByteArray toCrc;
    // toCrc.append(c, 5);
    // char crc = trashCrc(toCrc);
    // uchar a = 0x80;
    char m [2] = {0x00, 0x77};
    serialPortWritter(QByteArray(m, 2));
}

// qint16 crc16(QByteArray* data, int dataSize)
// {
//     qint16 crc = 0x0000; // initial CRC value
//     qint16 polynomial = 0x80; // CRC-16 polynomial

//     for (int i = 0; i < dataSize; i++)
//     {
//         crc ^= data[i].toInt(); // XOR data byte with current CRC
//         for (int j = 0; j < 8; j++)
//         {
//             if (crc & 0x0001) {
//                 crc >>= 1;
//                 crc ^= polynomial;
//             } else {
//                 crc >>= 1;
//             }
//         }
//     }

//     return crc;
// }



int MainWindow::trashCrc(QByteArray data)
{
    int sum = 0;
    for (int i = 0; i < data.size(); i++)
    {
        sum = sum + data[i];
    }
    sum = sum%256;
    char hex = static_cast<char>(sum);
    return hex;
}

