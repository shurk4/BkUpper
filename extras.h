#ifndef EXTRAS_H
#define EXTRAS_H


#pragma once

#include <QListWidget>
#include <QDateTime>

#include "nlohmann/json.hpp"

enum TypeMessage
{
    START, // Задача запущена(Главное окно записывает время запуска задачи)
    COMPLITE, // Задача завершена(Главное окно записывает время завершения задачи)
    ABORTED, // Задача прервана(Главное окно записывает время прерывания задачи)
    INFORMATION, // Просто информация
    LOG // Отправка лога(По окончанию задачи или при включённом отображении лога) гавное окно сохраняет лог в папке с копией
};

// Сообщение с информацией о задаче
struct taskMessage
{
    taskMessage();
    taskMessage(QString _name, TypeMessage _type, QString _message);

    QString name; // Имя задачи
    TypeMessage type; // Тип сообщения
    QString message; // Содержимое сообщения
};
Q_DECLARE_METATYPE(taskMessage)

using json = nlohmann::json;

class extras
{
public:
    extras();

    QString static secondsToString(qint64 seconds);

    void static showTasksList(QListWidget *_widget, json const _tasks);
    int static dayToInt(std::string const _day);
    int static monthToInt(std::string const _month);

    //общение между потоками
    static bool itsName(QString &_recivedMessage); // Проверка на запрос имени задачи
    static bool itsTime(QString &_recivedMessage); // Проверка, содержет ли запрос время задачи
};
#endif // EXTRAS_H
