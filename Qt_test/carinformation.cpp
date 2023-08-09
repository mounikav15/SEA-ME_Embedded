#include "carinformation.h"
#include <QDebug>

CarInformation::CarInformation(QObject *parent) : QObject(parent)
{
    speed = 0;
}

QString CarInformation::setSpeed(qreal speed)
{
    this->speed = speed;
    qDebug() << "Receive Data : " << speed;
    return "Good";
}


qreal CarInformation::getSpeed()
{
    return speed;
}

