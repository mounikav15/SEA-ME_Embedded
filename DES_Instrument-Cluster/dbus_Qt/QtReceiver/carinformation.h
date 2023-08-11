#ifndef CARINFORMATION_H
#define CARINFORMATION_H

#include <QObject>

class CarInformation : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.team4.Des02.CarInformation")
public:
    explicit CarInformation(QObject *parent = nullptr);

public Q_SLOTS:
    QString setSpeed(quint8 speed);
    QString setBattery(qreal battery);
    QString setGear(const QByteArray &gearByte);
    QString setRpm(quint8 rpm);
    quint8 getSpeed();
    qreal getBattery();
    QByteArray getGear();
    quint8 getRpm();

private:
    quint8 speed;
    qreal battery;
    char gear;
    quint8 rpm;
};

#endif // CARINFORMATION_H
