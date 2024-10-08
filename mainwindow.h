#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
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

    uchar marker[2] = {0xFF, 0xFF};

    uchar endMarker[2] = {0xFF,0xFF};

    QTimer *timer;
};
#endif // MAINWINDOW_H
