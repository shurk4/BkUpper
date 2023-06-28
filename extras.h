#ifndef EXTRAS_H
#define EXTRAS_H


#pragma once

#include <QListWidget>

#include "nlohmann/json.hpp"

using json = nlohmann::json;

class extras
{
public:
    extras();

    void static showTasksList(QListWidget *_widget, json const _tasks);
    int static dayToInt(std::string const _day);
    int static monthToInt(std::string const _month);
};

#endif // EXTRAS_H
