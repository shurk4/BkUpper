#ifndef ENGINE_H
#define ENGINE_H


 #pragma once

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QDir>
#include <QTime>
#include <string>

#include "extras.h"

class Engine : public QObject, public QRunnable
{
    Q_OBJECT

    QString taskName;
    QDir sourceDir;
    QDir destinationDir;

    bool forceStop = false;

    std::string log; // Лог
    int copyNum; // Количество копий для простого копирования

public:
    Engine();
    Engine(QString _sourceDir, QString _destinationDir, QString _taskName, int _copyNum = 0);
    ~Engine();

    void writeLog();
    void prepareMessage(QString message);

    void doWork();

    // проверка и создание каталога назначения
    void destDirCreate();
    // проверяет путь patch на наличие первой копии dir.
    // проверяет количество копий и если оно равное copyNum удаляет самую старую
    // Переименовывает копии для освобождения имени для текущей копии
    // Создаёт папку с именем copyName + 0 для текущей копии
    // принимает путь к первой копии bu0, путь расположения копий, количество копий
    //void optimizeFolders(QString &patch, QDir &dir, QString copyName, int &copyNum);
    void optimizeFolders(QDir &destinationDir, QString copyName, int &copyNum, std::string &log);

    // функция создаёт копию каталога по указанному пути
    void createFolder(QDir &sourceDir, QDir &destinationDir, std::string &log);

    // функция просматривает файлы в указанном каталоге и запускает их копирование
    void listEntriesFiles(QDir inSourceDir, QDir inDestinationDir, std::string &log);

    // функция просматривает каталоги по указанному пути и создаёт их копии
    void listEntriesDir(QDir inSourceDir, QDir inDestionationDir, std::string &log);

    // QRunnable interface
    void run();

signals:
    void sendMessage(taskMessage);

public slots:
    void recive(taskMessage _recive);
};

#endif // ENGINE_H
