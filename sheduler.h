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

#include <jsonconverter.h>
#include <extras.h>

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

public:
    Sheduler();

    void tasksUpdate();
    void run();

signals:
    void send(QString);

public slots:
    void recive(QString _recive);
    void reciveTasks(json _tasks);
};

#endif // SHEDULER_H
