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
                }
            }

            // Если месяц небыл отмечен, делаем todayTask = false
            if(monthTask) monthTask = false;
            else
            {
                continue;
            }
       }

       // При единовременном выполнении просто проверить дату
       if(taskJson["repeat"] == "once")
       {
           QString dateString = QString::fromStdString(taskJson["date"]) + " " + QString::fromStdString(taskJson["time"]);
           QDate tempDate(QDate::fromString(dateString));
           QDate currentDate(QDate::currentDate());

           if(tempDate.day() == currentDate.day() && tempDate.month() == currentDate.month() && tempDate.year() == currentDate.year())
           {
                todayTask = true;
           }
           else continue;
       }

       // Если задача выполнялась сегодня - пропускаем
//       if(i.value().contains("lastCompliteTime"))
//       {
//           QDateTime lastCompliteDate = QDateTime::fromString(QString::fromStdString(i.value()["lastCompliteTime"]), timeFormat);
//           if(lastCompliteDate.date() == QDate::currentDate())
//           {
//               prepareMessage(LOG, "Task: " + QString::fromStdString(i.key()) + " is completed today.");
//               continue;
//           }
//       }

       // Если задача пропущена и назначена на сегодня и не отмечена как выполняемая незамедлительно - пропускаем
       bool immediately = false;
       if(i.value().contains("immediately"))
       {
           immediately = i.value()["immediately"];
       }

       if(todayTask && QTime::fromString(QString::fromStdString(i.value()["time"])) < QTime::currentTime() && !immediately)
       {
           continue;
       }

       // Если задача прошла все условия и должна выполняться сегодня, добавляем её в спиок todayTasks
       if(todayTask)
       {
           prepareMessage( TypeMessage::LOG, QString::fromStdString(i.key()) + " добавлена в список задачь на сегодня");

           taskMessage("sheduler", TypeMessage::LOG, QString::fromStdString(i.key()) + " добавлена в список задачь на сегодня");

           QTime taskTime = QTime::fromString(QString::fromStdString(taskJson["time"]));
           todayTasks.insert(taskTime, QString::fromStdString(i.key()));
       }
    }

    taskMessage message("sheduler", TypeMessage::LOG, "\n\n---=== Задачи дня\n");

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
    emit send(taskMessage("sheduler", TypeMessage::LOG, "Sheduler is started\n"));
    while(true)
    {
        // Если прислан сигнал остановки потока
        if(forceStop)
        {
            QThread::sleep(4);
            break;
        }

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
    emit send(taskMessage("sheduler", TypeMessage::LOG, "Sheduler stoped\n"));
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
        prepareMessage(TypeMessage::LOG, "Task " + it.key() + " starting...!!!\n");
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

        QThreadPool::globalInstance()->start(task);
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

    // Отправить статус запрошенной задачи
    if(isTodayTask(_recive.name) && _recive.type == STATUS)
    {
        send(taskMessage(_recive.name, STATUS, "scheduled"));
    }

    if(_recive.type == ABORT)
    {
        taskMessage("all", ABORT, "Stop signal recived!");
        emit sendToTask(_recive);

        if(_recive.name == "all")
        {
            forceStop = true;
        }

        emit sendToTask(_recive);
    }
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
    prepareMessage(TypeMessage::LOG, "Task updated\n");

    taskMessage("sheduler", TypeMessage::LOG, "Task updated\n");
}
