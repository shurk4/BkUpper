#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#pragma once

#include <QObject>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QDir>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QStyle>
#include <QAction>
#include <QTextStream>
#include <map>

#include "jsonconverter.h"
#include "extras.h"
#include "engine.h"
#include "sheduler.h"

#include "taskseditwindow.h"
#include "settingswindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    JSONConverter configData;
    QString lastPath = QDir::homePath();

    QString timeFormat = "dd.MM.yyyy HH:mm:ss";
//    std::map<QString, Engine*> tasksMap;

    bool system = false;
    bool startWithOS = false;
    bool startMinimized = false;
    bool closeToTray = false;

    bool shedulerStarted = false;
    bool forceClose = false;

    SettingsWindow *settingsWindow;

    /* Объявляем объект будущей иконки приложения для трея */
    QSystemTrayIcon         * trayIcon;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void applyConfig();
    void iconActivate();
    void settingWindowStart();
    void restartSheduler();

protected:
    /* Виртуальная функция родительского класса в нашем классе
     * переопределяется для изменения поведения приложения,
     *  чтобы оно сворачивалось в трей, когда мы этого хотим
     */
    void closeEvent(QCloseEvent * event);

private slots:

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_pushButtonTasks_clicked();

    void on_pushButtonShowJSON_clicked();

    void on_pushButtonClearLog_clicked();

    void on_pushButtonTasksShow_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_pushButtonSystem_clicked();

    void on_pushButtonSettings_clicked();

    void on_actionSettings_triggered();

    void on_actionClose_triggered();

    void on_actionTray_triggered();

    void on_pushButtonRunThread_clicked();

    void on_pushButtonSendToThread_clicked();

    void on_pushButtonShedulerRestart_clicked();

    void on_pushButtonShedulerStop_clicked();

private:
    Ui::MainWindow *ui;

signals: // отправка данных
    void sendData(JSONConverter);
    void sendTasks(json);
    void send(taskMessage);

public slots: // Приём данных
    void reciveData(JSONConverter _data);
    void recive(taskMessage message);
    void showWindow();
};
#endif // MAINWINDOW_H
