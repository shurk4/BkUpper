#include "mainwindow.h"
#include "./ui_mainwindow.h"

/*
 * Типы сообщений taskMessage
 *
 * лог копирования(удалил при переносе функций)
 * сохранение лога в папку с копиями(имя лога: имя задачи_log.txt)
 * добавить точки выхода из потока при копировании(при выходе сохранять лог и указывать что задача была прервана)
 * записывать в config.json время последнего запуска и окончания задачи - сделано см. updateInfo.txt!
 * Добавить определение объёма копии
 *
 * поведение при условии хранения только одной копии
 *
 * привести в порядок MainWindow
 * сделать вывод процентов копирования и лога(если system активен)
 * Закрывать потоки при выходе из программы
 *
 * сделать автозагрузку
 * архивировать копии?
 * отправка сообщений
*/

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    configData.readConfig();

    ui->setupUi(this);

    applyConfig();

    iconActivate();

    extras::showTasksList(ui->listWidget, configData.getFullTasks());
}

MainWindow::~MainWindow()
{
    configData.writeConfig();
    emit send(taskMessage("all", TypeMessage::ABORTED, "stop"));
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
    /* Если окно видимо и чекбокс отмечен, то завершение приложения
     * игнорируется, а окно просто скрывается, что сопровождается
     * соответствующим всплывающим сообщением
     */
    if(this->isVisible() && closeToTray && !forceClose){
        event->ignore();
        this->hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);

        trayIcon->showMessage("Tray Program",
                              ("Приложение свернуто в трей. Для того чтобы, "
                                     "развернуть окно приложения, щелкните по иконке приложения в трее"),
                              icon,
                              2000);
    }
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason){
    case QSystemTrayIcon::Trigger:
        /* Событие игнорируется в том случае, если чекбокс не отмечен
         * */
        if(closeToTray){
            /* иначе, если окно видимо, то оно скрывается,
             * и наоборот, если скрыто, то разворачивается на экран
             * */
            if(!this->isVisible()){
                this->show();
            } else {
                this->hide();
            }
        }
        break;
    default:
        break;
    }

}

void MainWindow::applyConfig()
{
    system = configData.getConfigState("system");
    startWithOS = configData.getConfigState("startWithOS");
    startMinimized = configData.getConfigState("startMinimized");
    closeToTray = configData.getConfigState("closeToTray");

    lastPath = configData.getConfig("lastPath");

    if(system)
    {
        ui->verticalWidgetSystem->show();
    }
    else ui->verticalWidgetSystem->hide();
}

void MainWindow::iconActivate()
{
    /* Инициализируем иконку трея, устанавливаем иконку из набора системных иконок,
     * а также задаем всплывающую подсказку
     * */
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(this->style()->standardIcon(QStyle::SP_ComputerIcon));
    trayIcon->setToolTip("Tray Program" "\n"
                         "Работа со сворачиванием программы трей");
    /* После чего создаем контекстное меню из двух пунктов*/
    QMenu * menu = new QMenu(this);
    QAction * viewWindow = new QAction("Развернуть окно", this);
    QAction * quitAction = new QAction("Выход", this);

    /* подключаем сигналы нажатий на пункты меню к соответсвующим слотам.
     * Первый пункт меню разворачивает приложение из трея,
     * а второй пункт меню завершает приложение
     * */
    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    menu->addAction(viewWindow);
    menu->addAction(quitAction);

    /* Устанавливаем контекстное меню на иконку
     * и показываем иконку приложения в трее
     * */
    trayIcon->setContextMenu(menu);
    trayIcon->show();

    /* Также подключаем сигнал нажатия на иконку к обработчику
     * данного нажатия
     * */
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::settingWindowStart()
{
    settingsWindow = new SettingsWindow;
    settingsWindow->setModal(true);
    settingsWindow->show();

    connect(this, &MainWindow::sendData, settingsWindow, &SettingsWindow::reciveData); // Отправка данных из первого окна во второе

    connect(settingsWindow, &SettingsWindow::sendData, this, &MainWindow::reciveData); // Получение данных с второго окна

    emit sendData(configData);
}

void MainWindow::restartSheduler()
{
    if(shedulerStarted)
    {
        emit sendTasks(configData.getFullTasks());
    }
    else
    {
        Sheduler *sheduler = new Sheduler;

        connect(sheduler, &Sheduler::send, this, &MainWindow::recive);
        connect(this, &MainWindow::send, sheduler, &Sheduler::recive);
        connect(this, &MainWindow::sendTasks, sheduler, &Sheduler::reciveTasks);

        QThreadPool::globalInstance()->start(sheduler);
        emit sendTasks(configData.getFullTasks());
        shedulerStarted = true;
    }
}

void MainWindow::reciveData(JSONConverter _data)
{
    configData = _data;
    configData.writeConfig();
    extras::showTasksList(ui->listWidget, configData.getFullTasks());
}

void MainWindow::recive(taskMessage message)
{
    // Запись времени начала последнего выполнения задания
    if(message.type == START)
    {
        json task = configData.getTask(message.name);
        task["lastStartTime"] = QDateTime::currentDateTime().toString(timeFormat).toStdString();
        configData.setTask(message.name, task);
//        QDateTime::toTime_t(); // Время в сеундах от начала эпохи
    }

    // Запись времени окончания последнего выполнения задания
    if(message.type == COMPLITE)
    {
        json task = configData.getTask(message.name);
        QDateTime compliteTime = QDateTime::currentDateTime();
        task["lastCompliteTime"] = compliteTime.toString(timeFormat).toStdString();

        QDateTime startTime;
        startTime = QDateTime::fromString(QString::fromStdString(task["lastStartTime"]), timeFormat);

        // Запись времени выполнения задания
        QString execTime = extras::secondsToString(startTime.secsTo(compliteTime));
        task["execTime"] = execTime.toStdString();

        configData.setTask(message.name, task);

        ui->textEditLog->insertPlainText(execTime + "\n");
    }

    ui->textEditLog->insertPlainText(message.name + ": " + message.message + "\n");
}

void MainWindow::showWindow()
{
    this->show();
}

void MainWindow::on_pushButtonTasks_clicked()
{
    TasksEditWindow *taskWindow = new TasksEditWindow;
    taskWindow->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    taskWindow->setModal(true);

    hide();
    taskWindow->show();

    connect(this, &MainWindow::sendData, taskWindow, &TasksEditWindow::reciveData); // Отправка данных из первого окна во второе

    connect(taskWindow, &TasksEditWindow::sendData, this, &MainWindow::reciveData); // Получение данных с второго окна
    connect(taskWindow, &TasksEditWindow::showParentWindow, this, &MainWindow::showWindow); // Сигнал отображения окна

    emit sendData(configData);
}

void MainWindow::on_pushButtonSettings_clicked()
{
    settingWindowStart();
}

void MainWindow::on_pushButtonShowJSON_clicked()
{
    ui->textEditLog->insertPlainText(configData.getAllData());
}

void MainWindow::on_pushButtonClearLog_clicked()
{
    ui->textEditLog->clear();
}

void MainWindow::on_pushButtonTasksShow_clicked()
{
    extras::showTasksList(ui->listWidget, configData.getFullTasks());
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{

}

void MainWindow::on_pushButtonSystem_clicked()
{
    if(system)
    {
        ui->verticalWidgetSystem->hide();
        system = false;
        configData.addConfig("system", false);
    }
    else
    {
        ui->verticalWidgetSystem->show();
        system=true;
        configData.addConfig("system", true);
    }
}

void MainWindow::on_actionSettings_triggered()
{
    settingsWindow->show();
}

void MainWindow::on_actionClose_triggered()
{
    forceClose = true;
    close();
}

void MainWindow::on_actionTray_triggered()
{
    close();
}

void MainWindow::on_pushButtonRunThread_clicked()
{
//    tasksMap["task 1"] = new Engine("string1", "str2");

//    connect(tasksMap["task 1"], &Engine::sendMessage, this, &MainWindow::recive);
//    connect(this, &MainWindow::sendMessage, tasksMap["task 1"], &Engine::recive);

//    QThreadPool::globalInstance()->start(tasksMap["task 1"]);

//    ui->textEditTask->clear();
//    emit sendMessage("Test message");
}

void MainWindow::on_pushButtonSendToThread_clicked()
{

}

void MainWindow::on_pushButtonShedulerRestart_clicked()
{
    qDebug() << "MainWindow thread id: " << QThread::currentThreadId();
    ui->textEditLog->insertPlainText("Restart sheduler\n");
    restartSheduler();
}

void MainWindow::on_pushButtonShedulerStop_clicked()
{
    emit send(taskMessage("all", TypeMessage::ABORTED, "stop"));
    shedulerStarted = false;
}
