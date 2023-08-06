// mainwindow.cpp
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mydbusservice.h"
#include <QDBusConnection>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (!QDBusConnection::sessionBus().isConnected()) {
        qFatal("Cannot connect to the D-Bus session bus.");
        return;
    }

    if (!QDBusConnection::sessionBus().registerService("in.softprayog.add_server")) {
        qFatal("Cannot register service on the D-Bus session bus.");
        return;
    }

    MyDBusService *service = new MyDBusService(this);
    QDBusConnection::sessionBus().registerObject("/in/softprayog/adder", service, QDBusConnection::ExportAllSlots);
}

MainWindow::~MainWindow()
{
    delete ui;
}
