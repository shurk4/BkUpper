#include "engine.h"


Engine::Engine(QString _sourceDir, QString _destinationDir)
    : sourceDir(_sourceDir), destinationDir(_destinationDir)
{

}

Engine::~Engine()
{
    deleteLater();
}

void Engine::doWork()
{
    qDebug() << "Started thread" << QThread::currentThreadId();
    emit sendMessage("Started thread");


    // Запуск по расписанию
    QTime taskTime(20, 00);
    while(QTime::currentTime() < taskTime)
    {
        qDebug() << "Waiting to time" << QThread::currentThreadId();
        emit sendMessage("Waiting to time");
        QThread::sleep(5);
    }

    for(int i = 0; i < 10; i++)
    {
        qDebug() << "Work in process" << QThread::currentThreadId();
        emit sendMessage("Work is process");
        QThread::sleep(1);
    }
    qDebug() << "Work is finished";
    emit sendMessage("Work is finished");
}

void Engine::run()
{
    _sleep(100);
    doWork();
}

void Engine::recive(QString _recive)
{
    QString temp = "Thread recive message: ";
    temp += _recive + "\n";
    qDebug() << temp;
    emit sendMessage(temp);
}
