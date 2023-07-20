#include "sheduler.h"

/* "date": "26.06.2023",
            "days": [
                "monday",
                "tuesday",
                "wednesday",
                "thursday",
                "friday",
                "sunday"
            ],
            "destPath": "D:/",
            "months": [
                "march",
                "jule",
                "decembr"
            ],
            "repeat": "monthly",
            "sourcePath": "C:/",
            "time": "20:00",
            "type": "Зеркало" */

Sheduler::Sheduler(){}

Sheduler::~Sheduler()
{
//    prepareMessage("is stoped!");
    this->deleteLater();
}

// Составляет список задач на сегодня
void Sheduler::tasksUpdate()
{
    // Проходимся по списку задачь
    for(auto i = tasks.begin(); i != tasks.end(); i++)
    {
       bool todayTask = false;
       bool monthTask = false;
       json taskJson = i.value();

       // Проверка повторяемости задачи
       if(taskJson["repeat"] == "daily")
       {
           todayTask = true;
       }

       // Если задача повторяется еженедельно или ежемесячно
       if(taskJson["repeat"] == "weekly" || taskJson["repeat"] == "monthly")
       {
           // Проверяем отмечен ли текущий день
           for(auto it : taskJson["days"])
           {
               if(extras::dayToInt(it) == QDate::currentDate().dayOfWeek())
               {
                   todayTask = true; // Если всё ок, отмечаем что день совпадает
                   break;
               }
           }
       }

       // Если задача повторяется ежемесячно
       if(taskJson["repeat"] == "monthly")
       {
           // Проверяем отмечен ли текущий месяц
            for(auto it : taskJson["months"])
            {
                if(extras::monthToInt(it) == QDate::currentDate().month() && todayTask)
                {
                    monthTask = true; // Если всё ок, отмечаем
                    break;
                }
            }

            // Если месяц небыл отмечен, делаем todayTask = false
            if(monthTask) monthTask = false;
            else todayTask = false;
       }

       // При единовременном выполнении просто проверить дату
       if(taskJson["repeat"] == "once")
       {
           QString dateString = QString::fromStdString(taskJson["date"]) + " " + QString::fromStdString(taskJson["time"]);
           QDate tempDate(QDate::fromString(dateString));
//           emit send("Once task: " + QString::fromStdString(i.key()) + " " + tempDate.toString() + "\n");
           QDate currentDate(QDate::currentDate());

//           emit send("Temp Date & time: " + tempDate.toString() + "\n");
//           emit send("Current date & time: " + currentDate.toString() + "\n");

           if(tempDate.day() == currentDate.day() && tempDate.month() == currentDate.month() && tempDate.year() == currentDate.year())
           {
//               emit send("Once task is current day task: " + QString::fromStdString(i.key()) + "\n");
           }
       }

       // Выполнялась ли сегодня текущая задача
       if(i.value().contains("lastStartDate"))
       {
           QDate lastStartDate = QDate::fromString(QString::fromStdString(i.value()["lastStartDate"]));
           if(lastStartDate.day() == QDate::currentDate().day())
           {
               todayTask = false;
           }
       }

       // Если задача прошла все условия и должна выполняться сегодня, добавляем её в спиок todayTasks
       if(todayTask)
       {
           prepareMessage( TypeMessage::INFORMATION, QString::fromStdString(i.key()) + " добавлена в список задачь на сегодня");

           taskMessage("sheduler", TypeMessage::INFORMATION, QString::fromStdString(i.key()) + " добавлена в список задачь на сегодня");

           QTime taskTime = QTime::fromString(QString::fromStdString(taskJson["time"]));
           todayTasks.insert(taskTime, QString::fromStdString(i.key()));
       }
    }

    taskMessage message("sheduler", TypeMessage::INFORMATION, "\n\n---=== Задачи дня\n");

    for(auto it = todayTasks.begin(); it != todayTasks.end(); it++)
    {
        message.message += "Время: " + it.key().toString() + "\nИмя: " + it.value() + "\n";
    }
    emit send(message);
}

// метод действия в потоке
void Sheduler::run()
{
    qDebug() << "Sheduler is started in thread with id: " << QThread::currentThreadId() << "\n";
    emit send(taskMessage("sheduler", TypeMessage::INFORMATION, "Sheduler is started\n"));
    while(true)
    {
        // Если прислан сигнал остановки потока
        if(forceStop)
        {
            QThread::sleep(4);
            break;
        }

//        qDebug() << ".";

        // Если был обновлён список задач
        if(tasksUpdated)
        {
            tasksUpdate();
            tasksUpdated = false;
        }

        //Если пришло время выполнения задач
        if(taskReadyToRun())
        {
            runTasks();
        }
        QThread::sleep(2);
    }

    qDebug() << "Sheduler stoped\n";
    emit send(taskMessage("sheduler", TypeMessage::INFORMATION, "Sheduler stoped\n"));
    QThread::currentThread()->quit();
}

// Проверяет нет ли в списке задач на сегодня задачь для запуска и составялет список taskInWorks
bool Sheduler::taskReadyToRun()
{
    if(todayTasks.begin().key() <= QTime::currentTime())
    {
        tasksInWork.clear();

        for(auto it = todayTasks.begin(); it != todayTasks.end(); it++)
        {
            if(it.key() <= QTime::currentTime())
            {
                tasksInWork.insert(it.value(), QTime::currentTime());
                todayTasks.remove(it.key(), it.value());
            }
        }
        if(!tasksInWork.empty()) return true;
    }
    return false;
}

bool Sheduler::isTodayTask(QString _name)
{
    if(todayTasks.size() != 0)
    {    for(auto it = todayTasks.begin(); it != todayTasks.end(); it++)
        {
            if(it.value() == _name)
            {
                return true;
            }
        }
    }
    return false;
}

// Запустить задачи из списка tasksInWork
void Sheduler::runTasks()
{
    for(auto it = tasksInWork.begin(); it != tasksInWork.end(); it++)
    {
        qDebug() << "Task " << it.key() << " starting...!!!\n";
        prepareMessage(TypeMessage::INFORMATION, "Task " + it.key() + " starting...!!!\n");
        json currentTask = tasks[it.key().toStdString()];

        QString sourcePath = QString::fromStdString(currentTask["sourcePath"]);
        QString destPath = QString::fromStdString(currentTask["destPath"]);
        int copiesNum = 0;
        if(currentTask["type"] == "Копия")
        {
            copiesNum = currentTask["copiesNum"];
        }
        Engine *task = new Engine(sourcePath, destPath, it.key(), copiesNum);

        connect(task, &Engine::sendMessage, this, &Sheduler::reciveFromTask);
        connect(this, &Sheduler::send, task, &Engine::recive);

        debug("Bool log = " + QString::number(log));

        QThreadPool::globalInstance()->start(task);

        if(log)
        {
            prepareMessage(INFORMATION, "Try send log status to task");
            QThread::currentThread()->sleep(1);
            sendToTask(taskMessage(it.key(), LOG, "on"));
        }

        QThread::currentThread()->sleep(5);
    }
}

void Sheduler::debug(const QString str)
{
    qDebug() << "- sheduler message: " << str << "\n";
}

void Sheduler::prepareMessage(TypeMessage _type, QString _message)
{
    emit send(taskMessage("sheduler", _type , _message));
}

// Приём сообщения от родительского окна
void Sheduler::recive(taskMessage _recive)
{
    QString temp = "Sheduler recive message: " + _recive.name + " " + _recive.message;

    if(_recive.type == LOG)
    {
        if(_recive.message == "on")
        {
            log = true;
        }
        else
        {
            log = false;
        }
    }

//    // преадресация сообщения если задание выполняется
//    if(tasksInWork.size() != 0 && tasksInWork.contains(_recive.name))
//    {
//        emit sendToTask(_recive);
//    }

    // если задача запланирована на сегодня - сообщаем об этом
    if(isTodayTask(_recive.name) && log)
    {
        prepareMessage(LOG, "Task: " + _recive.name + " is scheduled today");
    }

    if(_recive.name == "all" && _recive.type == ABORT)
    {
        taskMessage("all", ABORT, "Stop signal recived!");
        emit sendToTask(_recive);
        forceStop = true;
    }

    taskMessage("sheduler",TypeMessage::INFORMATION, temp);

    emit sendToTask(_recive);
}

// Приём сообщения от запущеной задачи и отправка в родительское окно
void Sheduler::reciveFromTask(taskMessage _recive)
{
    emit send(_recive);
}

// Получение списка задачь с последующей обработкой
void Sheduler::reciveTasks(json _tasks)
{
    tasks = _tasks;
    tasksUpdated = true;
    prepareMessage(TypeMessage::INFORMATION, "Task updated\n");

    taskMessage("sheduler", TypeMessage::INFORMATION, "Task updated\n");
}
