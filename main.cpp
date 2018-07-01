#include "clientwindow.h"
#include "mainwindow.h"
#include "laadpaal.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    ClientWindow c;
    Laadpaal l;
    w.show();
    c.show();
    l.show();

    return a.exec();
}
