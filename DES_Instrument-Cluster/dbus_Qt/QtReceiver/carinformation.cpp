#include "carinformation.h"
#include <QDebug>

CarInformation::CarInformation(QObject *parent) : QObject(parent)
{
    speed = 0;
    battery = 0.0;
    gear = 'p';
}

QString CarInformation::setSpeed(qreal speed)
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

QString CarInformation::setGear(qreal gear)
{
    this->gear = gear;
    qDebug() << "Receive Gear Data : " << gear;
    return "Get Gear";
}

QString CarInformation::setRpm(qreal rpm)
{
    this->rpm = rpm;
    qDebug() << "Receive Rpm Data : " << rpm;
    return "Get Rpm";
}

qreal CarInformation::getSpeed()
{
    return speed;
}

qreal CarInformation::getBattery()
{
    return battery;
}

qreal CarInformation::getGear()
{
    return gear;
}

qreal CarInformation::getRpm()
{
    return rpm;
}
