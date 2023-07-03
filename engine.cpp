#include "engine.h"


Engine::Engine(QString _sourceDir, QString _destinationDir, QString _taskName, int _copyNum)
    : sourceDir(_sourceDir), destinationDir(_destinationDir), taskName(_taskName), copyNum(_copyNum)
{
    prepareMessage("Запущена");
}

Engine::~Engine()
{
    deleteLater();
}

void Engine::prepareMessage(QString message)
{
    emit sendMessage(taskMessage(taskName, message + "\n"));

}

void Engine::doWork()
{
    prepareMessage("Начинает работу");
    prepareMessage("Путь источник: " + sourceDir.absolutePath() + "\n");
    prepareMessage("Путь назначения: " + destinationDir.absolutePath() + "\n");

    destDirCreate();
}

void Engine::destDirCreate()
{
    prepareMessage("preparing the destination folder");

    if(!destinationDir.exists())
    {
        prepareMessage("Destination folder not exist!");

        QDir().mkdir(destinationDir.absolutePath());

        if(destinationDir.exists())
        {
            prepareMessage("Destination folder is created!");
        }
        else
        {
            prepareMessage("ERROR! Could not create destinaion folder!");
        }
    }
    else
    {
        prepareMessage("Destination folder is exist!");
    }
}

void Engine::run()
{
    doWork();
    if(forceStop) QThread::currentThread()->quit();
}

void Engine::recive(taskMessage _recive)
{
    if(_recive.name == taskName)
    {
        prepareMessage("Task recive message with name: " + _recive.name);
    }
    if(_recive.message == "stop") forceStop = true;
    prepareMessage("Task: is stopped!\n");
}
