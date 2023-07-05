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
    Engine(QString _sourceDir, QString _destinationDir, QString _taskName, int _copyNum = 1);
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
    void optimizeFolders();

    // функция создаёт копию каталога по указанному пути
    void createFolder(QDir _entryDir, QDir _targetDir);

    // функция просматривает каталоги по указанному пути и создаёт их копии - рекурсивный метод
    void listEntriesDir(QDir _entryDir, QDir _targetDir);

    // функция просматривает файлы в указанном каталоге и запускает их копирование
    void listEntriesFiles(QDir _entryDir, QDir _targetDir);

    // QRunnable interface
    void run();

signals:
    void sendMessage(taskMessage);

public slots:
    void recive(taskMessage _recive);
};

#endif // ENGINE_H
