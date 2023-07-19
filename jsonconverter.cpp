#include "jsonconverter.h"

JSONConverter::JSONConverter()
{

}

JSONConverter::JSONConverter(JSONConverter const &_other)
{
    data = _other.data;
    config = _other.config;
    tasks = _other.tasks;
}

JSONConverter::~JSONConverter()
{
//    writeConfig();
}

QString JSONConverter::getAllData()
{
    return QString::fromStdString(data.dump(4));
}

void JSONConverter::addConfig(QString _name, QString _value)
{
    config[_name.toStdString()] = _value.toStdString();
}

void JSONConverter::addConfig(QString _name, bool _value)
{
    config[_name.toStdString()] = _value;
}

QString JSONConverter::getConfig(QString _name)
{
    QString result = "";
    for(auto it = config.begin(); it != config.end(); it++)
        {
            if(it.key() == _name.toStdString())
                result = QString::fromStdString(it.value());
                break;
        }
    return result;
}

bool JSONConverter::getConfigState(QString _name)
{
    if(config.contains(_name.toStdString())) return config[_name.toStdString()];

    return false;
}

void JSONConverter::setFullConfig(json _config)
{
    config = _config;
}

json JSONConverter::getFullConfig()
{
    return config;
}

void JSONConverter::setFullTasks(json _tasks)
{
    tasks = _tasks;
}

json JSONConverter::getFullTasks()
{
    return tasks;
}

bool JSONConverter::containTask(const QString _name)
{
    return tasks.contains(_name.toStdString());
}

void JSONConverter::setTask(QString _name, json _task)
{
    tasks[_name.toStdString()] = _task;
    data["tasks"] = tasks;
//    qDebug() << QString::fromStdString(data.dump(4));
}

void JSONConverter::removeTask(QString _name)
{
    tasks.erase(_name.toStdString());
    data["tasks"] = tasks;
//    qDebug() << QString::fromStdString(tasks.dump(4));
}

json JSONConverter::getTask(QString _name)
{
    return tasks[_name.toStdString()];
}

bool JSONConverter::readConfig()
{
    std::ifstream file("config.json");
    if(!file.is_open())
    {
        data["error"] = "Файл не прочитан";
        return false;
    }
    if (!nlohmann::json::accept(file))
    {
        data["error"] = "Файл не соответствует структуре JSON";
        return false;
    }

    file.seekg(0, std::ios_base::beg); // Перенос курсора в начало последовательности данных

    file >> data;
    file.close();

    data["error"] = "Ошибок нет";

    config = data["config"];
    tasks = data["tasks"];
    return true;
}

bool JSONConverter::writeConfig()
{
    data["config"] = config;
    data["tasks"] = tasks;
    std::ofstream file("config.json");
    if(!file.is_open())
    {
        data["error"] = "Не удалось сохранить файл";
        return false;
    }
    file << std::setw(4) << data;
    file.close();

    qDebug() << "Config is writed";

    return true;
}
