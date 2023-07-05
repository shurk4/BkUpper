#include "engine.h"


Engine::Engine(QString _sourceDir, QString _destinationDir, QString _taskName, int _copyNum)
    : sourceDir(_sourceDir), destinationDir(_destinationDir), taskName(_taskName), copyNum(_copyNum)
{

}

Engine::~Engine()
{
    deleteLater();
}

void Engine::prepareMessage(QString message)
{
    emit sendMessage(taskMessage(taskName, message + "\n"));

}

void Engine::run()
{
    doWork();
    if(forceStop) QThread::currentThread()->quit();
}

void Engine::doWork()
{
    prepareMessage("Начинает работу");
    prepareMessage("Путь источник: " + sourceDir.absolutePath() + "\n");
    prepareMessage("Путь назначения: " + destinationDir.absolutePath() + "\n");    
    prepareMessage("Количество копий: " + QString::number(copyNum));

    destDirCreate();
    optimizeFolders();
    listEntriesDir(sourceDir, destinationDir);
    prepareMessage("Копирование завершено!");
    log += "Копирование завершено";
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

void Engine::optimizeFolders()
{
    QString buFolder = destinationDir.absolutePath() + "/" + taskName + "0";
    QDir dir = buFolder;

    if(dir.exists())
    {
        prepareMessage("Started operations with buckup folders!");

        // создание списка каталогов
        QStringList  entryFolders = destinationDir.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);

        prepareMessage("Have " + QString::number(entryFolders.size()) + " the backup copies\n");

        // Перечисление имеющихся копий
        foreach(QString entry, entryFolders)
        {
            prepareMessage(entry);
        }

        //!!!!!!!!!!!!!! Добавить поведение при условии хранения только одной копии!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // переименовывание каталогов и удаление старой копии
        for(int i = entryFolders.size(); i > 0; i--)
        {
            if(i == copyNum)
            {
                prepareMessage("Maximum numder copies reached");

                  QString oldName = destinationDir.absolutePath() + "/" + taskName + QString::number(i - 1);
                  QDir removePatch = oldName;
                  removePatch.removeRecursively();

                  prepareMessage("Old copy deleted");
            }
            else
            {
                QString oldName = taskName + QString::number(i - 1);
                QString newName = taskName + QString::number(i);

                destinationDir.rename(oldName, newName);

                prepareMessage("Folder " + oldName + " renamed to " + newName);
            }
        }
    }
    else
    {
        prepareMessage("Destination folder is not exist");
        QThread::currentThread()->quit();
    }

    QDir().mkdir(buFolder);
    destinationDir = buFolder;

    prepareMessage("Destination folder \"" + taskName + "0\" created");
}

void Engine::createFolder(QDir _entryDir, QDir _targetDir)
{
    // путь к копируемой папке
    QString newDirPatch = _targetDir.absolutePath() + "/" + _entryDir.dirName();
    // создание папки по указанному выше пути
    QDir().mkdir(newDirPatch);

    if(QDir(newDirPatch).exists())
    {
        prepareMessage("Folder created: " + newDirPatch);
        log += "Folder created: " + newDirPatch.toStdString() + "\n\n";
    }
    else
    {
        prepareMessage("!!! Folder not created!!! " + newDirPatch);
        log += "!!! Folder not created!!! " + newDirPatch.toStdString() + "\n";
    }
}

void Engine::listEntriesDir(QDir _entryDir, QDir _targetDir)
{
    createFolder(_entryDir, _targetDir);

    // создание списка каталогов
    QStringList  entryFolders = _entryDir.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);

    listEntriesFiles(_entryDir, _targetDir);

    // обработка списка каталогов
    foreach(QString  entry, entryFolders)
    {
        QString absPatch = _entryDir.absolutePath() + "/" + entry; // добавляем в исходный путь текущую папку списка вложений
        QDir entryDir = absPatch;

        QDir targetDir = _targetDir.absolutePath() + "/" + _entryDir.dirName();

        listEntriesDir(entryDir, targetDir);
    }
}

void Engine::listEntriesFiles(QDir _entryDir, QDir _targetDir)
{
    QString source = _entryDir.absolutePath();
    QString dest = _targetDir.absolutePath() + "/" + _entryDir.dirName();
    // создание списка файлов
    QStringList entryFiles = _entryDir.entryList(QDir::Files | QDir::Hidden/* |QDir::NoDotAndDotDot*/);

    foreach(QString  entry, entryFiles)
    {
        int count = 0;
        bool succes = false;
        log += "Copy file\n";
        prepareMessage("Copy file");

        QFile sourceFile = _entryDir.absolutePath() + "/" + entry;
        QString destFile = dest + "/" + entry;
        do
        {
            log += "    " + sourceFile.fileName().toStdString() + "\n";
            prepareMessage("    " + sourceFile.fileName() + "\n");

            if(sourceFile.copy(destFile))
            {
                succes = true;
                prepareMessage("Succes!");
                log += "Succes!\n";
            }
            else
            {
                prepareMessage("!!! Fail!");
                prepareMessage(sourceFile.errorString());
                log += "Fail!\n";

                 if(count < 5)
                 {
                     QThread::currentThread()->sleep(5);
                     log+= "Try again!";
                     count++;
                 }
            }
            prepareMessage("\n");
            log += "\n";
        } while (!succes && count < 5);
    }
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
