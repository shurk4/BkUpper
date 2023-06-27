#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H


#pragma once

#include <QObject>
#include <QWidget>
#include <QMessageBox>

#include "jsonconverter.h"

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void on_checkBoxStartWithOS_stateChanged(int arg1);

    void on_checkBoxStartMinimized_stateChanged(int arg1);

    void on_checkBoxMinimizeToTray_stateChanged(int arg1);

    void on_pushButtonOk_clicked();

private:
    Ui::SettingsWindow *ui;

    JSONConverter data;

protected:
    /* Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     *  чтобы оно сворачивалось в трей, когда мы этого хотим
     */
    void closeEvent(QCloseEvent * event);

signals: // отправка данных
    void sendData(JSONConverter);

public slots: // Приём данных
    void reciveData(JSONConverter _data);
};

#endif // SETTINGSWINDOW_H
