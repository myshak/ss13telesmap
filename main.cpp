#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("ss13telescience");
    QCoreApplication::setApplicationName("Space Station 13 Telescience Manager");
    MainWindow w;
    w.show();
    
    return a.exec();
}
