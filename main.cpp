#include "clientwindow.h"
#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ClientWindow c;
    w.show();
    c.show();

    return a.exec();
}
