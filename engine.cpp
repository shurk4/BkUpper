#include "engine.h"


Engine::Engine(QString _sourceDir, QString _destinationDir, QString _taskName, int _copyNum)
    : sourceDir(_sourceDir), destinationDir(_destinationDir), taskName(_taskName), copyNum(_copyNum)
{

}

Engine::~Engine()
{
    deleteLater();
}

void Engine::prepareMessage(TypeMessage _type, QString _message)
{
    // Отправляет информацию выполняемой задачи шедулеру и шедулер переправляет её в главное окно
    emit sendMessage(taskMessage(taskName, _type , _message + "\n"));
}

void Engine::run()
{
    doWork();
    if(forceStop) QThread::currentThread()->quit();
}

void Engine::doWork()
{
    prepareMessage(TypeMessage::START, "Начинает работу");
    prepareMessage(TypeMessage::INFORMATION, "Путь источник: " + sourceDir.absolutePath() + "\n");
    prepareMessage(TypeMessage::INFORMATION, "Путь назначения: " + destinationDir.absolutePath() + "\n");
    prepareMessage(TypeMessage::INFORMATION, "Максимальное количество копий: " + QString::number(copyNum));

    destDirCreate();
    optimizeFolders();
    listEntriesDir(sourceDir, destinationDir);
    prepareMessage(TypeMessage::COMPLITE, "Копирование завершено!");
    log += "Копирование завершено";
}

void Engine::destDirCreate()
{
    prepareMessage(TypeMessage::INFORMATION, "preparing the destination folder");

    if(!destinationDir.exists())
    {
        prepareMessage(TypeMessage::INFORMATION, "Destination folder not exist!");

        QDir().mkdir(destinationDir.absolutePath());

        if(destinationDir.exists())
        {
            prepareMessage(TypeMessage::INFORMATION, "Destination folder is created!");
        }
        else
        {
            prepareMessage(TypeMessage::INFORMATION, "ERROR! Could not create destinaion folder!");
        }
    }
    else
    {
        prepareMessage(TypeMessage::INFORMATION, "Destination folder is exist!");
    }
}

void Engine::optimizeFolders()
{
    QString buFolder = destinationDir.absolutePath() + "/" + taskName + "0";
    QDir dir = buFolder;

    if(dir.exists())
    {
        prepareMessage(TypeMessage::INFORMATION, "Started operations with buckup folders!");

        // создание списка каталогов
        QStringList  entryFolders = destinationDir.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);

        prepareMessage(TypeMessage::INFORMATION, "Найдено копий: " + QString::number(entryFolders.size()) + "\n");

        // Перечисление имеющихся копий
        foreach(QString entry, entryFolders)
        {
            prepareMessage(TypeMessage::INFORMATION, entry);
        }

        //!!!!!!!!!!!!!! Добавить поведение при условии хранения только одной копии!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // переименовывание каталогов и удаление старой копии

        if(entryFolders.size() >= copyNum)
        {
            prepareMessage(TypeMessage::INFORMATION, "Maximum numder copies reached");
        }

        for(int i = entryFolders.size(); i > 0; i--)
        {
            if(i >= copyNum)
            {
                  QString oldName = destinationDir.absolutePath() + "/" + entryFolders[i - 1];
                  QDir removePatch = oldName;
                  removePatch.removeRecursively();

                  prepareMessage(TypeMessage::INFORMATION, "Old copy deleted");
            }
            else
            {
                QString oldName = taskName + QString::number(i - 1);
                QString newName = taskName + QString::number(i);

                destinationDir.rename(oldName, newName);

                prepareMessage(TypeMessage::INFORMATION, "Folder " + oldName + " renamed to " + newName);
            }
        }
    }
    else
    {
        prepareMessage(TypeMessage::INFORMATION, "Destination folder is not exist");
        QThread::currentThread()->quit();
    }

    QDir().mkdir(buFolder);
    destinationDir.setPath(buFolder);

    prepareMessage(TypeMessage::INFORMATION, "Destination folder \"" + taskName + "0\" created");
}

void Engine::createFolder(QDir _entryDir, QDir _targetDir)
{
    // путь к копируемой папке
    QString newDirPatch = _targetDir.absolutePath() + "/" + _entryDir.dirName();
    // создание папки по указанному выше пути
    QDir().mkdir(newDirPatch);

    if(QDir(newDirPatch).exists())
    {
        prepareMessage(TypeMessage::INFORMATION, "Folder created: " + newDirPatch);
        log += "Folder created: " + newDirPatch.toStdString() + "\n\n";
    }
    else
    {
        prepareMessage(TypeMessage::INFORMATION, "!!! Folder not created!!! " + newDirPatch);
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
        prepareMessage(TypeMessage::INFORMATION, "Copy file");

        QFile sourceFile = _entryDir.absolutePath() + "/" + entry;
        QString destFile = dest + "/" + entry;
        do
        {
            log += "    " + sourceFile.fileName().toStdString() + "\n";
            prepareMessage(TypeMessage::INFORMATION, "    " + sourceFile.fileName() + "\n");

            if(sourceFile.copy(destFile))
            {
                succes = true;
                prepareMessage(TypeMessage::INFORMATION, "Succes!");
                log += "Succes!\n";
            }
            else
            {
                prepareMessage(TypeMessage::INFORMATION, "!!! Fail!");
                prepareMessage(TypeMessage::INFORMATION, sourceFile.errorString());
                log += "Fail!\n";

                 if(count < 5)
                 {
                     QThread::currentThread()->sleep(5);
                     log+= "Try again!";
                     count++;
                 }
            }
            prepareMessage(TypeMessage::INFORMATION, "\n");
            log += "\n";
        } while (!succes && count < 5);
    }
}

void Engine::recive(taskMessage _recive)
{
    if(_recive.name == taskName)
    {
        prepareMessage(TypeMessage::INFORMATION, "Task recive message with name: " + _recive.name);
    }
    if(_recive.message == "stop") forceStop = true;
    prepareMessage(TypeMessage::ABORTED, "Task: is stopped!\n");
}
