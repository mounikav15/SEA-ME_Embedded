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
    QString setSpeed(qint16 speed);
    QString setBattery(qreal battery);
    QString setGear(qreal gear);
    QString setRpm(qint16 rpm);
    qreal getSpeed();
    qreal getBattery();
    qreal getGear();
    qreal getRpm();

private:
    qint16 speed;
    double battery;
    char gear;
    qint16 rpm;
};

#endif // CARINFORMATION_H
