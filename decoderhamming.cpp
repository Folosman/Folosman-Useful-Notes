#include "decoderhamming.h"

#include <QDebug>

DecoderHamming::DecoderHamming(QObject *parent)
    : QObject{parent}
{

}

QByteArray DecoderHamming::decode(const QByteArray &data)
{

    QByteArray decodedData;
    for(char byte : data)
    {
        uchar byteValue = static_cast<uchar>(byte);

        int b1 = (byteValue >> 6) & 1;
        int b2 = (byteValue >> 5) & 1;
        int b3 = (byteValue >> 4) & 1;
        int b4 = (byteValue >> 3) & 1;
        int b5 = (byteValue >> 2) & 1;
        int b6 = (byteValue >> 1) & 1;
        int b7 = (byteValue) & 1;

        int s1 = b1 ^ b3 ^ b5 ^ b7;
        int s2 = b2 ^ b3 ^ b6 ^ b7;
        int s3 = b4 ^ b5 ^ b6 ^ b7;

        int syndrom = (s3 << 2) | (s2 << 1) | s1;

        if(syndrom != 0)
        {
            byteValue = byteValue ^ (1 << (7 - syndrom));
        }

        int data = ((byteValue >> 5) & 1) << 3 |
                   ((byteValue >> 3) & 1) << 2 |
                   ((byteValue >> 2) & 1) << 1 |
                   ((byteValue >> 1) & 1);

        decodedData.append(static_cast<char>(data));
    }
    return decodedData;
}
