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
    qreal getSpeed();

private:
    int speed;
};

#endif // CARINFORMATION_H
