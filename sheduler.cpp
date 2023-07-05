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
    prepareMessage("is stoped!");
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
           prepareMessage(QString::fromStdString(i.key()) + " добавлена в список задачь на сегодня");

           taskMessage("sheduler", QString::fromStdString(i.key()) + " добавлена в список задачь на сегодня");

           QTime taskTime = QTime::fromString(QString::fromStdString(taskJson["time"]));
           todayTasks.insert(taskTime, QString::fromStdString(i.key()));
       }
    }

    taskMessage message("sheduler", "\n\n---=== Задачи дня\n");

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
    emit send(taskMessage("sheduler", "Sheduler is started\n"));
    while(true)
    {
        // Если прислан сигнал остановки потока
        if(forceStop)
        {
            QThread::sleep(4);
            break;
        }

        qDebug() << ".";

        // Если был обновлён список задач
        if(tasksUpdated)
        {
            tasksUpdate();
            tasksUpdated = false;
        }

        //Если пришло время выполнения задач
        if(isTodayTask())
        {
            runTasks();
        }
        QThread::sleep(2);
    }

    qDebug() << "Sheduler stoped\n";
    emit send(taskMessage("sheduler", "Sheduler stoped\n"));
    QThread::currentThread()->quit();
}

// Запустить задачи из списка tasksInWork
void Sheduler::runTasks()
{
    for(auto it = tasksInWork.begin(); it != tasksInWork.end(); it++)
    {
        qDebug() << "Task " << it.key() << " starting...!!!\n";
        prepareMessage("Task " + it.key() + " starting...!!!\n");

        QString sourcePath = QString::fromStdString(tasks[it.key().toStdString()]["sourcePath"]);
        QString destPath = QString::fromStdString(tasks[it.key().toStdString()]["destPath"]);
        Engine *task = new Engine(sourcePath, destPath, it.key(), 3);

        connect(task, &Engine::sendMessage, this, &Sheduler::reciveFromTask);
        connect(this, &Sheduler::send, task, &Engine::recive);

        QThreadPool::globalInstance()->start(task);
    }
}

// Проверяет нет ли в списке задач на сегодня задачь для запуска и составялет список taskInWorks
bool Sheduler::isTodayTask()
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

void Sheduler::prepareMessage(QString _message)
{
    emit send(taskMessage("sheduler", _message));
}

// Приём сообщения от родительского окна
void Sheduler::recive(taskMessage _recive)
{
    QString temp = "Sheduler recive message: " + _recive.name + _recive.message;
    if(_recive.message == "stop")
    {
//        prepareMessage("Stop signal recived!");
        taskMessage("sheduler", "Stop signal recived!");
        emit sendToTask(_recive);
        forceStop = true;

//        QThread::currentThread()->terminate();
//        QThread::currentThread()->quit();
    }

    temp += "\n";
    qDebug() << temp;
//    prepareMessage(temp);
    taskMessage("sheduler", temp);
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
    prepareMessage("Task updated\n");

    taskMessage("sheduler", "Task updated\n");
}
