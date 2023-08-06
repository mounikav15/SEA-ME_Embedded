// mydbusservice.h
#include <QObject>
#include <QDebug>

class MyDBusService : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "in.softprayog.dbus_example")

public:
    MyDBusService(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

public slots:
    QString add_numbers(const QString &data)
    {
        qDebug() << "Received data: " << data;
        return "Response from Qt server";
    }
};
