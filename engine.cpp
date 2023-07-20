#include "engine.h"


Engine::Engine(QString _sourceDir, QString _destinationDir, QString _taskName, int _copyNum)
    : sourceDir(_sourceDir), destinationDir(_destinationDir), taskName(_taskName), copyNum(_copyNum)
{
    log = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm") + "\n";
}

Engine::~Engine()
{
    deleteLater();
}

void Engine::writeLog()
{
    QFile file(destinationDir.absolutePath() + "/log.txt");
    if(file.isOpen())
    {
        file.write(log.toUtf8());
        file.close();
    }
}

void Engine::prepareMessage(TypeMessage _type, QString _message)
{
    // Отправляет информацию выполняемой задачи шедулеру и шедулер переправляет её в главное окно
    if(_type == LOG && lifeLog) emit sendMessage(taskMessage(taskName, _type , _message + "\n"));
    else emit sendMessage(taskMessage(taskName, _type , _message + "\n"));
}

void Engine::run()
{
    doWork();
    if(forceStop) QThread::currentThread()->quit();
}

void Engine::doWork()
{
    qDebug() << "Task " << taskName << "started in thread: " << QThread::currentThreadId() << "\n";
    prepareMessage(TypeMessage::START, "Начинает работу");
    prepareMessage(TypeMessage::LOG, "Путь источник: " + sourceDir.absolutePath() + "\n");
    prepareMessage(TypeMessage::LOG, "Путь назначения: " + destinationDir.absolutePath() + "\n");
    prepareMessage(TypeMessage::LOG, "Максимальное количество копий: " + QString::number(copyNum));

    destDirCreate();
    optimizeFolders();
    listEntriesDir(sourceDir, destinationDir);
    prepareMessage(TypeMessage::COMPLITE, "Копирование завершено!");
    log += "Копирование завершено";
}

void Engine::destDirCreate()
{
    prepareMessage(TypeMessage::LOG, "preparing the destination folder");

    if(!destinationDir.exists())
    {
        prepareMessage(TypeMessage::LOG, "Destination folder not exist!");

        QDir().mkdir(destinationDir.absolutePath());

        if(destinationDir.exists())
        {
            prepareMessage(TypeMessage::LOG, "Destination folder is created!");
        }
        else
        {
            prepareMessage(TypeMessage::LOG, "ERROR! Could not create destinaion folder!");
        }
    }
    else
    {
        prepareMessage(TypeMessage::LOG, "Destination folder is exist!");
    }
}

void Engine::optimizeFolders()
{
    QString buFolder = destinationDir.absolutePath() + "/" + taskName + "0";
    QDir dir = buFolder;

    if(dir.exists())
    {
        prepareMessage(TypeMessage::LOG, "Started operations with buckup folders!");

        // создание списка каталогов
        QStringList  entryFolders = destinationDir.entryList(QDir::AllDirs | QDir::Hidden | QDir::NoDotAndDotDot);

        prepareMessage(TypeMessage::LOG, "Найдено копий: " + QString::number(entryFolders.size()) + "\n");
        prepareMessage(COPIES, QString::number(entryFolders.size()));

        // Перечисление имеющихся копий
        foreach(QString entry, entryFolders)
        {
            prepareMessage(TypeMessage::LOG, entry);
        }

        //!!!!!!!!!!!!!! Добавить поведение при условии хранения только одной копии!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // переименовывание каталогов и удаление старой копии

        if(entryFolders.size() >= copyNum)
        {
            prepareMessage(TypeMessage::LOG, "Maximum numder copies reached");
        }

        for(int i = entryFolders.size(); i > 0; i--)
        {
            if(i >= copyNum)
            {
                  QString oldName = destinationDir.absolutePath() + "/" + entryFolders[i - 1];
                  QDir removePatch = oldName;
                  removePatch.removeRecursively();

                  prepareMessage(TypeMessage::LOG, "Old copy deleted");
            }
            else
            {
                QString oldName = taskName + QString::number(i - 1);
                QString newName = taskName + QString::number(i);

                destinationDir.rename(oldName, newName);

                prepareMessage(TypeMessage::LOG, "Folder " + oldName + " renamed to " + newName);
            }
        }
    }
    else
    {
        prepareMessage(TypeMessage::LOG, "Destination folder is not exist");
        QThread::currentThread()->quit();
    }

    QDir().mkdir(buFolder);
    destinationDir.setPath(buFolder);

    prepareMessage(TypeMessage::LOG, "Destination folder \"" + taskName + "0\" created");
}

void Engine::createFolder(QDir _entryDir, QDir _targetDir)
{
    // путь к копируемой папке
    QString newDirPatch = _targetDir.absolutePath() + "/" + _entryDir.dirName();
    // создание папки по указанному выше пути
    QDir().mkdir(newDirPatch);

    if(QDir(newDirPatch).exists())
    {
        prepareMessage(TypeMessage::LOG, "Folder created: " + newDirPatch);
        log += "Folder created: " + newDirPatch + "\n\n";
    }
    else
    {
        prepareMessage(TypeMessage::LOG, "!!! Folder not created!!! " + newDirPatch);
        log += "!!! Folder not created!!! " + newDirPatch + "\n";
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
        prepareMessage(TypeMessage::LOG, "Copy file");

        QFile sourceFile = _entryDir.absolutePath() + "/" + entry;
        QString destFile = dest + "/" + entry;
        do
        {
            log += "    " + sourceFile.fileName() + "\n";
            prepareMessage(TypeMessage::LOG, "    " + sourceFile.fileName() + "\n");

            if(sourceFile.copy(destFile))
            {
                succes = true;
                prepareMessage(TypeMessage::LOG, "Succes!");
                log += "Succes!\n";
            }
            else
            {
                prepareMessage(TypeMessage::LOG, "!!! Fail!");
                prepareMessage(TypeMessage::LOG, sourceFile.errorString());
                log += "Fail!\n";

                 if(count < 5)
                 {
                     QThread::currentThread()->sleep(5);
                     log+= "Try again!";
                     count++;
                 }
            }
            prepareMessage(TypeMessage::LOG, "\n");
            log += "\n";
        } while (!succes && count < 5);
    }
}

void Engine::recive(taskMessage _recive)
{
    qDebug() << "Task: " << taskName << " recive message: " << _recive.name << " " << _recive.message << "Log = " << QString::number(lifeLog) << "\n";
    prepareMessage(INFORMATION, " recived message: " + _recive.name + " " + _recive.message + " log = " + QString::number(lifeLog));

    if(_recive.type == ABORT && (_recive.name == taskName || _recive.name == "all"))
    {
        forceStop = true;
        prepareMessage(TypeMessage::ABORT, "Task: is stopped!\n");
        return;
    }

    if(_recive.name == taskName && _recive.type == LOG)
    {
        qDebug() << taskName << " log ";
        if(_recive.message == "on")
        {
            lifeLog = true;
            qDebug() << "enabled";
        }
        else
        {
            lifeLog = false;
            qDebug() << "disabled";
        }
        qDebug() << "\n";
    }

    prepareMessage(INFORMATION, "Lifelog = " + QString::number(lifeLog));
}
