#include "extras.h"


extras::extras(){}

QString extras::secondsToString(qint64 seconds)
{
    const qint64 DAY = 86400;
    qint64 days = seconds / DAY;
    QTime t = QTime(0,0).addSecs(seconds % DAY);
//    return QString("%1 days, %2:%3:%4") .arg(days).arg(t.hour()).arg(t.minute()).arg(t.second());
    return QString("%1 days, " + t.toString()) .arg(days);
}

QString extras::cutPath(const QString _path, const int stringSize)
{
    int leftPartSize = _path.indexOf('/') + 4;
    int rightPartSize = stringSize - leftPartSize - 3;

//    return _path.left(leftPartSize) + "..." + _path.right(_path.size() - _path.lastIndexOf("/") + 4);
    return _path.left(leftPartSize) + "..." + _path.right(rightPartSize);
}

void extras::showTasksList(QListWidget *_widget, const json _tasks)
{
    _widget->clear();
    for(auto it = _tasks.begin(); it != _tasks.end(); it++)
    {
        QListWidgetItem *newItem = new QListWidgetItem;
        newItem->setText(QString::fromStdString(it.key()));
        _widget->addItem(newItem);
    }
}

int extras::dayToInt(const std::string _day)
{
    if(_day == "monday") return 1;
    else if(_day == "tuesday") return 2;
    else if(_day == "wednesday") return 3;
    else if(_day == "thursday") return 4;
    else if(_day == "friday") return 5;
    else if(_day == "saturday") return 6;
    else if(_day == "sunday") return 7;
    else return 0;
}

int extras::monthToInt(const std::string _month)
{
    if(_month == "january") return 1;
    else if(_month == "february") return 2;
    else if(_month == "march") return 3;
    else if(_month == "april") return 4;
    else if(_month == "may") return 5;
    else if(_month == "june") return 6;
    else if(_month == "jule") return 7;
    else if(_month == "august") return 8;
    else if(_month == "september") return 9;
    else if(_month == "october") return 10;
    else if(_month == "november") return 11;
    else if(_month == "decembr") return 12;
    else return 0;
}

bool extras::itsName(QString &_recivedMessage)
{
    if(_recivedMessage.contains("name:"))
    {
        _recivedMessage.remove("name:");
        return true;
    }
    return false;
}

bool extras::itsTime(QString &_recivedMessage)
{
    if(_recivedMessage.contains("time"))
    {
        _recivedMessage.remove("time:");
        return true;
    }
    return false;
}

taskMessage::taskMessage()
{

}

taskMessage::taskMessage(QString _name, TypeMessage _type, QString _message) : name(_name), type(_type), message(_message)
{

}
