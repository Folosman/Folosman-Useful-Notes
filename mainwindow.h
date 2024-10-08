#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
// #include <QThread>
#include "readrzm.h"
#include "decoderhamming.h"
#include "calculationoutput.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString serialPortReader();
    void serialPortWritter(const QByteArray& begin);
    void pushBtn();
    bool ReadToWritte();
    QDataStream serialData();
    uint16_t crc16(uint16_t* data, int dataSize);
    qint16 calculateCrc(const QByteArray &data);
    quint16 calculateCrc16(const QByteArray &data);
    quint16  alternativeCrc(QByteArray data);
    quint16 wikiCrc16(QByteArray data);

    quint16 kirillCrc16(QByteArray data);
    quint16 neuroCrc16(QByteArray data);
    int trashCrc(QByteArray data);//////////////
    QByteArray byteStaffing(QByteArray &data);
    void sendPackages(QByteArray& data);
private:
    Ui::MainWindow *ui;
    QSerialPort *serialport;

    ReadRzm readRzm;

    DecoderHamming decoder;

    CalculationOutput calculationOutput;

    uchar marker[2] = {0x1B, 0x80};

    uchar endMarker[2] = {0x1B,0x03};




























    // 0x01
    QByteArray portFreqInsert;//unsigned short

    //0x02
    QByteArray portTelemetryEnable;//bool

    //0x03
    QByteArray portSpectreEnable;//bool

    //0x04
    QByteArray receiverValue;
    QByteArray portFreq;//unsigned int
    QByteArray portRegim;//bool
    QByteArray portSpectre;//bool
    QByteArray portRssi;//int
    QByteArray portVoltageUsb;//int
    QByteArray portVoltageMc;//int
    QByteArray portVoltageRu;//int
    QByteArray portVoltageOu;//int
    QByteArray portTemperature;//int

    //0x10
    QByteArray telemetryValue;
    QByteArray timeOut;//int
    QByteArray portTelemetryRssi;//int
    QByteArray portTelemetryFreq;//int
    QByteArray portTelemetryDispFreq;//int
    QByteArray portTelemetry;//QByteArray

    //0x11
    QByteArray spectrumValue;
    QByteArray portDataSpectre;

    QTimer *timer;
};

// class InitializationSerialPort : public QObject
// {
//     Q_OBJECT
//     QThread initThread;
// public:
//     InitializationSerialPort()
//     {
//         MainWindow *init = new MainWindow;
//         init->moveToThread(&initThread);
//         connect(&initThread, &QThread::finished, init, &QObject::deleteLater);
//     }
//     ~InitializationSerialPort()
//     {
//         initThread.quit();
//         initThread.wait();
//     }
// };
#endif // MAINWINDOW_H
