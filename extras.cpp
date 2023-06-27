#include "extras.h"


extras::extras()
{

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
