#include "calculationoutput.h"
#include <QtMath>
#include <QDebug>


CalculationOutput::CalculationOutput(QObject *parent)
    : QObject{parent}
{

}

std::tuple<double, double, double> CalculationOutput::enuToAes(float a, float b, float c)
{
    double slantRange       = sqrt(pow(a, 2) + pow(b, 2) + pow(c, 2));
    double zConjugate       = qAtan2(sqrt(pow(a, 2) + pow(b, 2)), c);
    double elevationAngle   = M_PI / 2 - zConjugate;

    double azimuth = qAtan2(a, b);

    if(azimuth < 0.0)
    {
        azimuth += 2 * M_PI;
    }
    return std::make_tuple(azimuth, elevationAngle, slantRange);
}

std::tuple<double, double, double> CalculationOutput::geodeticToAes(double lat, double lon, double alt,
                                                                    double lat_org, double lon_org, double alt_org)
{
    auto [TempOne, TempTwo, TempThree] = CalculationOutput::geodeticToenu(lat, lon, alt, lat_org, lon_org, alt_org);
    auto [tempOne, tempTwo, tempThree] = CalculationOutput::enuToAes(TempOne, TempTwo, TempThree);

    return std::make_tuple(tempOne, tempTwo, tempThree);
}

std::tuple<double, double, double> CalculationOutput::ecefToGeodetic(double x, double y, double z)
{
    PJ_CONTEXT *context = proj_context_create();
    PJ  *transformer    = proj_create_crs_to_crs(context,
                                             "+proj=geocent +ellps=WGS84 +datum=WGS84",
                                             "+proj=latlon +ellps=WGS84 +datum=WGS84",
                                             NULL);

    if(transformer == nullptr)
    {
        qDebug() << "Error transformer not created";
        return std::make_tuple(0.0, 0.0, 0.0);
    }

    PJ *transfromerDegrees = proj_normalize_for_visualization(context, transformer);
    /*  destructor  */
    proj_destroy(transformer);
    transformer = transfromerDegrees;


    if(transformer == nullptr)
    {
        qDebug() << "Error transformer not created";
        return std::make_tuple(0.0, 0.0, 0.0);
    }

    PJ_COORD ecefCoord      = proj_coord(x, y, z, 0);
    PJ_COORD geodeticCoord  = proj_trans(transformer, PJ_FWD, ecefCoord);


    proj_destroy(transformer);
    proj_context_destroy(context);


    return std::make_tuple(geodeticCoord.lpzt.phi, geodeticCoord.lpzt.lam, geodeticCoord.lpzt.z);
}


std::tuple<double, double, double> CalculationOutput::geodeticToenu(double lat, double lon, double alt,
                                    double lat_org, double lon_org, double alt_org)
{
    PJ_CONTEXT *context = proj_context_create();
    PJ *transformer     = proj_create_crs_to_crs(context,
                                           "+proj=latlong +ellps=WGS84 +datum=WGS84",
                                           "+proj=geocent +ellps=WGS84 +datum=WGS84",
                                           NULL);

    if(transformer == nullptr)
    {
        qDebug() << "Error transformer not created";
        return {0.0, 0.0, 0.0};

    }

    PJ_COORD geodeticCoord  = proj_coord(lon, lat, alt, 0);
    PJ_COORD ecefCoord      = proj_trans(transformer, PJ_FWD, geodeticCoord);

    PJ_COORD geodeticCoordOrg   = proj_coord(lon_org, lat_org, alt_org, 0);
    PJ_COORD ecefCoordOrg       = proj_trans(transformer, PJ_FWD, geodeticCoordOrg);


    /*      Destructors     */
    proj_destroy(transformer);
    proj_context_destroy(context);


    double dx = ecefCoord.xyz.x - ecefCoordOrg.xyz.x;
    double dy = ecefCoord.xyz.y - ecefCoordOrg.xyz.y;
    double dz = ecefCoord.xyz.z - ecefCoordOrg.xyz.z;


    double sin_lat = std::sin(DEG_TO_RAD * lat_org);
    double cos_lat = std::cos(DEG_TO_RAD * lat_org);
    double sin_lon = std::sin(DEG_TO_RAD * lon_org);
    double cos_lon = std::cos(DEG_TO_RAD * lon_org);


    std::array<std::array<double, 3>, 3> rotMatrix =
        {{
            {-sin_lon, cos_lon,0},
            {-sin_lat * cos_lon, -sin_lat * sin_lon, cos_lat},
            {cos_lat * cos_lon, cos_lat * sin_lon, sin_lat}
        }};


    double enuX = rotMatrix[0][0] * dx + rotMatrix[0][1] * dy + rotMatrix[0][2] * dz;
    double enuY = rotMatrix[1][0] * dx + rotMatrix[1][1] * dy + rotMatrix[1][2] * dz;
    double enuZ = rotMatrix[2][0] * dx + rotMatrix[2][0] * dy + rotMatrix[2][0] * dz;

    return std::make_tuple(enuX, enuY, enuZ);
}

std::tuple<double, double, double> ecefToElli(double x, double y, double z)
{
    float earthA = 6378137.0;               // расстояние до центра земли по экватору
    float earthB = 6356752.31424518;        // расстояние до центра земли на полюсе

    double earth_a2_b2  = earthA * earthA - earthB * earthB;
    double e2       = earth_a2_b2 / (earthA * earthA);
    double ee2      = earth_a2_b2 / (earthB * earthB);

    double xCoord = x / 100.0;
    double yCoord = y / 100.0;
    double zCoord = z / 100.0;

    double lam  = qAtan2(yCoord, xCoord);
    double p    = sqrt(pow(xCoord, 2) + pow(yCoord, 2));
    double t    = qAtan2(zCoord * earthA, p * earthB);
    double phi  = qAtan2(zCoord + ee2 * earthB * pow(qSin(t), 3), p - e2 * earthA * pow(qCos(t), 3));

    double R        = earthA / sqrt(1 - e2 * pow(qSin(phi), 2));
    double height   = p / qCos(phi) - R;
    double latitude = phi * 180.0 / M_PI;
    double longitude = lam * 180.0 / M_PI;

    return std::make_tuple(height, latitude, longitude);
}


std::tuple<int, int, int> CalculationOutput::timeOutput(int seconds)
{
    int second  = seconds % 60;
    int minutes = seconds / 60 % 60;
    int hours   = seconds / 3600;

    return std::make_tuple(second, minutes, hours);
}

void CalculationOutput::checkingServiceability(int usbVoltage, int mcuVoltage, int lna1Voltage, int lna2Voltage)
{
    if (!(5000 * 0.9 <= usbVoltage <= 5000 * 1.1))
        qDebug() << "USB вышел из строя";
    else if(!(3300 * 0.9 <= mcuVoltage <= 3300 * 1.1))
        qDebug() << "MCU вышел из строя";
    else if(!(3000 * 0.95 <= lna1Voltage <= 3000 * 1.05))
        qDebug() << "lna1 вышел из строя";
    else if(!(5000 * 0.8 <= lna2Voltage <= 5000 * 1.2))
        qDebug() << "lna2 вышел из строя";
    else
        qDebug() << "Состояние аппаратуры в пределах нормы";
}



            /*          !!!БЕЗУМИЕ!!!           */

QString CalculationOutput::latitudeProcessing(float latitudeValue)
{
    int degree      = (int)latitudeValue;
    double minutes  = (latitudeValue - degree) * 60;
    int seconds     = ((minutes - (int)minutes) * 60);

    QString degreeStr   = QString::number(degree);
    QString minutesStr  = QString::number((int)minutes);
    QString secondsStr  = QString::number(seconds);

    if(degreeStr.length() < 2)
    {
        degreeStr = "0";

        for(int i = 0; i < 2 - QString(degree).length(); ++i)
        {
            degreeStr += "0";
        }

        degreeStr += QString(degree);
    }

    if(minutesStr.length() < 2)
    {
        QString minutesStr = "0";

        for(int i = 0; i < 2 - QString(minutesStr).length(); ++i)
        {
            minutesStr += "0";
        }

        minutesStr += QString::number(minutes);
    }

    if(secondsStr.length() < 2)
    {
        QString secondsStr = "0";

        for(int i = 0; i < 2 - QString(seconds).length(); ++i)
        {
            secondsStr += "0";
        }

        secondsStr += QString::number(seconds);
    }

    return degreeStr + "°" + minutesStr + "'" + secondsStr + '"';
}

QString CalculationOutput::latitudeProcessing(float latitudeValue, char a)
{
    int degree      = (int)latitudeValue;
    double minutes  = (latitudeValue - degree) * 60;
    int seconds     = ((minutes - (int)minutes) * 60);

    QString degreeStr   = QString::number(degree);
    QString minutesStr  = QString::number((int)minutes);
    QString secondsStr  = QString::number(seconds);

    if(degreeStr.length() < 2)
    {
        degreeStr = "0";

        for(int i = 0; i < 2 - QString(degree).length(); ++i)
        {
            degreeStr += "0";
        }

        degreeStr += QString(degree);
    }

    if(minutesStr.length() < 2)
    {
        QString minutesStr = "0";

        for(int i = 0; i < 2 - QString(minutesStr).length(); ++i)
        {
            minutesStr += "0";
        }

        minutesStr += QString::number(minutes);
    }

    if(secondsStr.length() < 2)
    {
        QString secondsStr = "0";

        for(int i = 0; i < 2 - QString(seconds).length(); ++i)
        {
            secondsStr += "0";
        }

        secondsStr += QString::number(seconds);
    }

    return QString(a) + " " + degreeStr + "°" + minutesStr + "'" + secondsStr + '"';
}

QString CalculationOutput::longitudeProcessing(float longitudeValue)
{
    int degree      = (int)longitudeValue;
    double minutes  = (longitudeValue - degree) * 60;
    int seconds     = ((minutes - (int)minutes) * 60);

    QString degreeStr   = QString::number(degree);
    QString minutesStr  = QString::number((int)minutes);
    QString secondsStr  = QString::number(seconds);

    if(degreeStr.length() < 2)
    {
        degreeStr = "0";

        for(int i = 0; i < 2 - QString(degree).length(); ++i)
        {
            degreeStr += "0";
        }

        degreeStr += QString(degree);
    }

    if(minutesStr.length() < 2)
    {
        QString minutesStr = "0";

        for(int i = 0; i < 2 - QString(minutesStr).length(); ++i)
        {
            minutesStr += "0";
        }

        minutesStr += QString::number(minutes);
    }

    if(secondsStr.length() < 2)
    {
        QString secondsStr = "0";

        for(int i = 0; i < 2 - QString(seconds).length(); ++i)
        {
            secondsStr += "0";
        }

        secondsStr += QString::number(seconds);
    }

    QString sign;
    longitudeValue >= 0?sign = "+":sign = " ";

    return sign + " " + degreeStr + "°" + minutesStr + "'" + secondsStr + '"';
}

QString CalculationOutput::longitudeProcessing(float longitudeValue, char a)
{
    int degree      = (int)longitudeValue;
    double minutes  = (longitudeValue - degree) * 60;
    int seconds     = ((minutes - (int)minutes) * 60);

    QString degreeStr   = QString::number(degree);
    QString minutesStr  = QString::number((int)minutes);
    QString secondsStr  = QString::number(seconds);

    if(degreeStr.length() < 2)
    {
        degreeStr = "0";

        for(int i = 0; i < 2 - QString(degree).length(); ++i)
        {
            degreeStr += "0";
        }

        degreeStr += QString(degree);
    }

    if(minutesStr.length() < 2)
    {
        QString minutesStr = "0";

        for(int i = 0; i < 2 - QString(minutesStr).length(); ++i)
        {
            minutesStr += "0";
        }

        minutesStr += QString::number(minutes);
    }

    if(secondsStr.length() < 2)
    {
        QString secondsStr = "0";

        for(int i = 0; i < 2 - QString(seconds).length(); ++i)
        {
            secondsStr += "0";
        }

        secondsStr += QString::number(seconds);
    }

    QString sign;
    longitudeValue >= 0?sign = "+":sign = " ";

    return QString(a) + sign + " " + degreeStr + "°" + minutesStr + "'" + secondsStr + '"';
}

QString CalculationOutput::azimuthProcessing(float azimuthValue)
{
    int degree      = (int)azimuthValue;
    double minutes  = (azimuthValue - degree) * 60;
    int seconds     = ((minutes - (int)minutes) * 60);

    QString degreeStr   = QString::number(degree);
    QString minutesStr  = QString::number((int)minutes);
    QString secondsStr  = QString::number(seconds);

    if(degreeStr.length() < 2)
    {
        degreeStr = "0";

        for(int i = 0; i < 2 - QString(degree).length(); ++i)
        {
            degreeStr += "0";
        }

        degreeStr += QString(degree);
    }

    if(minutesStr.length() < 2)
    {
        QString minutesStr = "0";

        for(int i = 0; i < 2 - QString(minutesStr).length(); ++i)
        {
            minutesStr += "0";
        }

        minutesStr += QString::number(minutes);
    }

    if(secondsStr.length() < 2)
    {
        QString secondsStr = "0";

        for(int i = 0; i < 2 - QString(seconds).length(); ++i)
        {
            secondsStr += "0";
        }

        secondsStr += QString::number(seconds);
    }

    return degreeStr + "°" + minutesStr + "'" + secondsStr + '"';
}


double CalculationOutput::displaySignalPercentLevel(int rssi)
{
    percent = (rssi + 130) * 2.0 / 3.0;

    if (percent < 0)
    {
        percent = 0;
    }
    else if(percent > 100)
    {
        percent = 100;
    }

    return percent;
}
