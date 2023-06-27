#ifndef TASKSEDITWINDOW_H
#define TASKSEDITWINDOW_H

#pragma once

#include <QObject>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>

#include "nlohmann/json.hpp"
#include "jsonconverter.h"
#include "extras.h"

namespace Ui {
class TasksEditWindow;
}

class TasksEditWindow : public QDialog
{
    Q_OBJECT

    JSONConverter data;
    json task;
    QString lastPath = QDir::homePath();

protected:
    /* Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     *  чтобы оно сворачивалось в трей, когда мы этого хотим
     */
    void closeEvent(QCloseEvent * event);

public:
    explicit TasksEditWindow(QWidget *parent = nullptr);
    ~TasksEditWindow();

    void saveTask();
    void showTask(QString const _name);
    void clearAllData();

    void hideWidgets();

    bool daySelected();
    bool monthSelected();
    void clearDays();
    void clearMonths();

    json daysToJson();
    void daysFromJson();

    json monthsToJson();
    void monthsFromJson();

private:
    Ui::TasksEditWindow *ui;

    bool system = false;
    QSize currentSize;

private slots:

    void on_pushButtonSourcePath_clicked();

    void on_pushButtonDestPath_clicked();

    void on_pushButtonSave_clicked();

    void on_radioButtonDaily_clicked();

    void on_radioButtonWeekly_clicked();

    void on_radioButtonMonthly_clicked();

    void on_radioButtonOnce_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButtonDeleteTask_clicked();

    void on_pushButtonSystem_clicked();

signals: // отправка данных
    void sendData(JSONConverter);
    void showParentWindow();

public slots: // Приём данных
    void reciveData(JSONConverter _data);
};

#endif // TASKSEDITWINDOW_H
