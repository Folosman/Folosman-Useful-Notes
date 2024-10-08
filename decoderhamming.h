#ifndef DECODERHAMMING_H
#define DECODERHAMMING_H

#include <QObject>

class DecoderHamming : public QObject
{
    Q_OBJECT
public:
    explicit DecoderHamming(QObject *parent = nullptr);

    QByteArray decode(const QByteArray &input);
private:

signals:
};

#endif // DECODERHAMMING_H
