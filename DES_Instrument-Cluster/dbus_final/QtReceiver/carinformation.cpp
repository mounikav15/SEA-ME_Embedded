#include "carinformation.h"
#include <QDebug>

CarInformation::CarInformation(QObject *parent) : QObject(parent)
{
    speed = 0;
    battery = 0.0;
    gear = 'P';
}

QString CarInformation::setSpeed(quint8 speed)
{
    this->speed = speed;
    qDebug() << "Receive Speed Data : " << speed;
    return "Get Speed";
}

QString CarInformation::setBattery(qreal battery)
{
    this->battery = battery;
    qDebug() << "Receive Battery Data : " << battery;
    return "Get Battery";
}

QString CarInformation::setGear(quint8 gearByte)
{
    gear = static_cast<char>(gearByte);
    qDebug() << "Receive Gear Data : " << gear;
    return "Get Gear";
}

QString CarInformation::setRpm(quint8 rpm)
{
    this->rpm = rpm;
    qDebug() << "Receive Rpm Data : " << rpm;
    return "Get Rpm";
}

quint8 CarInformation::getSpeed()
{
    return speed;
}

qreal CarInformation::getBattery()
{
    return battery;
}

char CarInformation::getGear()
{
    return gear;
}

quint8 CarInformation::getRpm()
{
    return rpm;
}
