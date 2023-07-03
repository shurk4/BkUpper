#ifndef EXTRAS_H
#define EXTRAS_H


#pragma once

#include <QListWidget>

#include "nlohmann/json.hpp"

struct taskMessage
{
    taskMessage();
    taskMessage(QString _name, QString _message);

    QString name;
    QString type;
    QString message;
};
Q_DECLARE_METATYPE(taskMessage)

using json = nlohmann::json;

class extras
{
public:
    extras();

    void static showTasksList(QListWidget *_widget, json const _tasks);
    int static dayToInt(std::string const _day);
    int static monthToInt(std::string const _month);

    //общение между потоками
    static bool itsName(QString &_recivedMessage); // Проверка на запрос имени задачи
    static bool itsTime(QString &_recivedMessage); // Проверка, содержет ли запрос время задачи
};
#endif // EXTRAS_H
