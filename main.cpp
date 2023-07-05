#include "mainwindow.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    int metaReg = qRegisterMetaType<taskMessage>(); //Регистрация структуры(метаобъекта) taskMessage(extras.cpp)
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
