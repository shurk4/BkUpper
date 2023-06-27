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
};

#endif // EXTRAS_H
