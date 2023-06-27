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

class Engine : public QObject, public QRunnable
{
    Q_OBJECT

    QString sourceDir;
    QString destinationDir;

    std::string log;

public:
    Engine();
    Engine(QString _sourceDir, QString _destinationDir);
    ~Engine();

    void writeLog();

    void doWork();

    // QRunnable interface
public:
    void run();

signals:
    void sendMessage(QString);

public slots:
    void recive(QString _recive);
};

#endif // ENGINE_H
