#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H


#pragma once

#include <QString>
#include <QDateTime>
#include <QDebug>

#include <fstream>

#include "nlohmann/json.hpp"

enum TaskType
{
    BACKUP
};

struct task
{
    QString name;
    TaskType taskType;
    QDateTime time;
    QString sourcePath;
    QString destPath;
    QString shedule;
};

using json = nlohmann::json;

class JSONConverter
{
    json data;
    json config;
    json tasks;
public:
    JSONConverter();
    JSONConverter(JSONConverter const &_other);
    ~JSONConverter();

    bool readConfig();
    bool writeConfig();

    QString getAllData();

    void addConfig(QString _name, QString _value);
    void addConfig(QString _name, bool _value);
    QString getConfig(QString _name);    
    bool getConfigState(QString _name);

    void setFullConfig(json _config);
    json getFullConfig();

    void setFullTasks(json _tasks);
    json getFullTasks();

    void setTask(QString _name, json _task);
    void removeTask(QString _name);
    json getTask(QString _name);
};

#endif // JSONCONVERTER_H
