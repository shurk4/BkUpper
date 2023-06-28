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

void Sheduler::tasksUpdate()
{
    qDebug() <<  "tasks.sieze = " << tasks.size() << "\n";
    emit send("tasks.sieze = " + QString::number(tasks.size()) + "\n");

    for(auto i = tasks.begin(); i != tasks.end(); i++)
    {
       bool todayTask = false;
       bool monthTask = false;
       json taskJson = i.value();

       emit send("\n\n");

       if(taskJson["repeat"] == "daily")
       {
           todayTask = true;
           emit send("Dayly task: " + QString::fromStdString(i.key()) + "\n");
       }
       if(taskJson["repeat"] == "weekly" || taskJson["repeat"] == "monthly")
       {
           for(auto it : taskJson["days"])
           {
               if(extras::dayToInt(it) == QDate::currentDate().dayOfWeek())
               {
                   todayTask = true;
                   emit send("Current day task: " + QString::fromStdString(i.key()) + "\n");
                   break;
               }
           }
       }
       if(taskJson["repeat"] == "monthly")
       {
            emit send("Task: " + QString::fromStdString(i.key()) + " - is monthly\n");
            for(auto it : taskJson["months"])
            {
                QString tempString = "task month number:" + QString::number(extras::monthToInt(it)) + "\n";
                tempString += "current month number:" + QString::number(QDate::currentDate().month()) + "\n";

                emit send(tempString);

                if(extras::monthToInt(it) == QDate::currentDate().month() && todayTask)
                {
                    emit send("Current month task: " + QString::fromStdString(i.key()) + "\n");
                    monthTask = true;
                    break;
                }
                else
                {
                    emit send("Month " + QString::fromStdString(it) + " false: " + QString::fromStdString(i.key()) + "\n");
                }
            }
            if(monthTask) monthTask = false;
            else todayTask = false;
       }
       if(taskJson["repeat"] == "once")
       {
           QString dateString = QString::fromStdString(taskJson["date"]) + " " + QString::fromStdString(taskJson["time"]);
           QDate tempDate(QDate::fromString(dateString));
           emit send("Once task: " + QString::fromStdString(i.key()) + " " + tempDate.toString() + "\n");
           QDate currentDate(QDate::currentDate());

           emit send("Temp Date & time: " + tempDate.toString() + "\n");
           emit send("Current date & time: " + currentDate.toString() + "\n");

           if(tempDate.day() == currentDate.day() && tempDate.month() == currentDate.month() && tempDate.year() == currentDate.year())
           {
               emit send("Once task is current day task: " + QString::fromStdString(i.key()) + "\n");
           }
       }

       if(todayTask)
       {
           emit send("\n--- Last if(todayTask): " + QString::fromStdString(i.key()) + "\n");

           QTime taskTime = QTime::fromString(QString::fromStdString(taskJson["time"]));
           todayTasks.insert(taskTime, QString::fromStdString(i.key()));
       }
    }

    emit send("\n\n---=== Задачи дня\n");
    for(auto it = todayTasks.begin(); it != todayTasks.end(); it++)
    {
        emit send("Время: " + it.key().toString() + "\nИмя: " + it.value() + "\n");
    }
}

void Sheduler::run()
{
    while(true)
    {
//        if(forceStop) QThread::;

        qDebug() << "Sheduler is work\n";
        emit send("Sheduler is work\n");
        if(tasksUpdated)
        {
            tasksUpdate();
            tasksUpdated = false;
        }
        QThread::sleep(2);
    }

    deleteLater();
}

void Sheduler::recive(QString _recive)
{
    QString temp = "Thread recive message: ";
    if(_recive == "stop")
    {
        forceStop = true;
    }
    temp += _recive + "\n";
    qDebug() << temp;
    emit send(temp);
}

void Sheduler::reciveTasks(json _tasks)
{
    tasks = _tasks;
    tasksUpdated = true;
    emit send("Task updated\n");
}

