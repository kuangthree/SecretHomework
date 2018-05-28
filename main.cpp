#include "mainwindow.h"
#include <QApplication>
#include "encryptfilter.h"
#include "decryptfilter.h"
#include<QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle(QObject::tr("KK加密王 V1.0"));
    w.show();
    return a.exec();
}
