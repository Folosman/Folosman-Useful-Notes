#ifndef CALCULATIONOUTPUT_H
#define CALCULATIONOUTPUT_H

#include <QObject>
#include <cmath>
#include <proj.h>

const double PI_RAD = M_PI / 180;

class CalculationOutput : public QObject
{
    Q_OBJECT
public:
    explicit CalculationOutput(QObject *parent = nullptr);

    std::tuple<double, double, double> enuToAes(float a, float b, float c);
    std::tuple<double, double, double> ecefToGeodetic(double x, double y, double z);
    std::tuple<double, double, double> geodeticToAes(double lat, double lon, double alt,
                                                     double lat_org, double lon_org, double alt_org);

    std::tuple<double, double, double> geodeticToenu(double lat, double lon, double alt,
                                       double lat_org, double lon_org, double alt_org);

    std::tuple<double, double, double> ecefToElli(double x, double y, double z);
    std::tuple<int, int, int> timeOutput(int second);

    QString latitudeProcessing(float latitudeValue);
    QString latitudeProcessing(float latitudeValue, char a);

    QString longitudeProcessing(float longitudeValue);
    QString longitudeProcessing(float lonigtudeValue, char a);

    QString azimuthProcessing(float azimuthValue);


    void checkingServiceability(int usbVoltage, int mcuVoltage, int lna1Voltage, int lna2Voltage);

    double displaySignalPercentLevel(int rssi);

private:
    double percent;

signals:
};

#endif // CALCULATIONOUTPUT_H
