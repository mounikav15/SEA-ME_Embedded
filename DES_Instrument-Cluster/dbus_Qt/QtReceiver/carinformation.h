#ifndef CARINFORMATION_H
#define CARINFORMATION_H

#include <QObject>

class CarInformation : public QObject
{
    Q_OBJECT
public:
    explicit CarInformation(QObject *parent = nullptr);

public Q_SLOTS:
    QString setSpeed(qreal speed);
    QString setBattery(qreal battery);
    QString setGear(qreal gear);
    qreal getSpeed();
    qreal getBattery();
    qreal getGear();

private:
    int speed;
    double battery;
    char gear;
};

#endif // CARINFORMATION_H
