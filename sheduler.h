#ifndef SHEDULER_H
#define SHEDULER_H

#pragma once

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QDir>
#include <QTime>
#include <string>

#include "jsonconverter.h"
#include "extras.h"
#include "engine.h"

//struct task
//{
//    QString name;
//    TaskType taskType;
//    QDateTime time;
//    QString sourcePath;
//    QString destPath;
//    QString shedule;
//};

class Sheduler : public QObject, public QRunnable
{
    Q_OBJECT

    bool forceStop = false;
    bool tasksUpdated = false;
    json tasks;
    QMultiMap<QTime, QString> todayTasks;
    QMultiMap<QString, QTime> tasksInWork;

public:
    Sheduler();
    ~Sheduler();

    void tasksUpdate(); // Составляет список задач на сегодня
    void run(); // метод действия в потоке
    void runTasks(); // Запустить задачи из списка tasksInWork
    void debug(QString const str);

    bool taskReadyToRun();  // Проверяет нет ли в списке задач на сегодня задачь для запуска и составялет список taskInWorks
    bool isTodayTask(QString _name);

    void prepareMessage(const TypeMessage _type, const QString _message);

signals:
    void send(taskMessage); // Отправка сообщения родительскому окну
    void sendToTask(taskMessage); // Отправка сообщения задаче

public slots:
    void recive(taskMessage _recive); // Приём сообщения от родительского окна
    void reciveFromTask(taskMessage _recive); // Приём сообщения от запущеной задачи и отправка в родительское окно
    void reciveTasks(json _tasks); // Приём списка задачь с последующей обработкой
};

#endif // SHEDULER_H
