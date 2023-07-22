#include "mainwindow.h"
#include "./ui_mainwindow.h"

/*
 * Типы сообщений taskMessage
 *
 * Проверить/сделать обновление задачь в планировщике после добавления задачи
 * Проверить/сделать применение настроек после нажатия кнопки ОК в настройках
 * Проверить/сделать "Запускать свёрнутым"
 *
 * Добавить запрос и обработку статуса выбранной задачи(выполняется, выполнена сегодня и т.д.
 * Добавить в лог время непредвиденного завершения программы и обработку этого события после перезапуска программы
 *  (если время завершения нет или оно более раннее чем время начала)
 * добавить точки выхода из потока при копировании(при выходе сохранять лог и указывать что задача была прервана)
 * Добавить определение объёма копии
 *
 * поведение при условии хранения только одной копии
 *
 * переделать IF-ы на SWTCH-и
 *
 * привести в порядок MainWindow
 * сделать вывод процентов копирования и лога(если system активен)
 * Закрывать потоки при выходе из программы
 *
 * создавать иконку в трее только при активации фонового режима
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

    iconActivate(); // Создаёт иконку в трее

    ui->gridWidgetTaskInfo->hide();

    extras::showTasksList(ui->listWidget, configData.getFullTasks());

    this->resize(100, 100);
    restartSheduler();
}

MainWindow::~MainWindow()
{
    configData.writeConfig();
    emit send(taskMessage("all", TypeMessage::ABORT, "stop"));
    delete ui;
}

// обмен данными
void MainWindow::prepareMessage(const QString _name, const TypeMessage _type, const QString _message)
{
    taskMessage message(_name, _type, _message);
    send(message);
}

void MainWindow::reciveData(JSONConverter _data)
{
    configData = _data;
    configData.writeConfig();
    extras::showTasksList(ui->listWidget, configData.getFullTasks());
//    sendData(configData);
}

void MainWindow::recive(taskMessage message)
{
    QString toLog = "";

    if(configData.containTask(message.name))
    {
        json task = configData.getTask(message.name);

        if(message.type == START) // Запись времени начала последнего выполнения задания
        {
            QString time = QDateTime::currentDateTime().toString(timeFormat);
            task["lastStartTime"] = time.toStdString();

            if(logs.contains(message.name)) logs[message.name].clear();
            toLog += "Started at: " + time + "\n";
        }
        else if(message.type == COMPLITE) // Запись времени окончания последнего выполнения задания
        {
            QDateTime compliteTime = QDateTime::currentDateTime();
            task["lastCompliteTime"] = compliteTime.toString(timeFormat).toStdString();
            toLog += "Complited at: " + compliteTime.toString(timeFormat) + "\n";

            QDateTime startTime;
            startTime = QDateTime::fromString(QString::fromStdString(task["lastStartTime"]), timeFormat);

            // Запись времени выполнения задания
            QString execTime = extras::secondsToString(startTime.secsTo(compliteTime));
            task["execTime"] = execTime.toStdString();

            toLog += "Execution time: " + execTime + "\n";
            toLog += "Complite status: " + message.message + "\n";

            writeLog(message.name);
            writeLog("sheduler");
        }

        // запись количества имеющихся копий
        if(message.type == COPIES)
        {
            task["currentCopiesNum"] = message.message.toInt();
        }

        configData.setTask(message.name, task);
    }

    if(message.type == LOG) // Просто лог
    {
        toLog += message.message + "\n";
    }

    logs[message.name] += toLog;

    if(system && selectedTaskName == message.name)
    {
        ui->textEditLog->insertPlainText(toLog);
    }
}

// обработка события завершения приложения и сворачивание в трей
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
    switch (reason)
    {
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

// основные функции
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
        prepareMessage("all", ABORT, "");
        QThread::currentThread()->sleep(3);
    }

    Sheduler *sheduler = new Sheduler;

    connect(sheduler, &Sheduler::send, this, &MainWindow::recive);
    connect(this, &MainWindow::send, sheduler, &Sheduler::recive);
    connect(this, &MainWindow::sendTasks, sheduler, &Sheduler::reciveTasks);

    QThreadPool::globalInstance()->start(sheduler);
    emit sendTasks(configData.getFullTasks());
    shedulerStarted = true;
}

void MainWindow::showWindow()
{
    this->show();
}

void MainWindow::showTaskLog()
{
    if(logs.contains(selectedTaskName) && system)
    {
        ui->textEditLog->clear();
        ui->textEditLog->setText(logs[selectedTaskName]);
        if(logScrol)
        {
            QTextCursor cursor = ui->textEditLog->textCursor();
            cursor.movePosition(QTextCursor::End);
            ui->textEditLog->setTextCursor(cursor);
        }
    }
}

void MainWindow::writeLog(const QString _name)
{
    QDir dir("logs");
    if(!dir.exists()) QDir().mkdir("logs");

    if(logs.contains(_name))
    {
        QFile file("logs/" + _name + ".log");
        if(file.open(QIODevice::ReadWrite))
        {
            file.write(logs[_name].toUtf8());
            file.close();
        }
    }
}

// кнопки
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

void MainWindow::on_pushButtonSystem_clicked()
{
    if(system)
    {
        ui->verticalWidgetSystem->hide();
        system = false;
        configData.addConfig("system", false);
        ui->textEditLog->clear();
    }
    else
    {
        ui->verticalWidgetSystem->show();
        system=true;
        configData.addConfig("system", true);

        showTaskLog();
    }

    this->resize(100, 100);
}

void MainWindow::on_actionSettings_triggered()
{
    settingWindowStart();
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

void MainWindow::on_pushButtonShedulerRestart_clicked()
{
    qDebug() << "MainWindow thread id: " << QThread::currentThreadId();
    ui->textEditLog->insertPlainText("Restart sheduler\n");
    restartSheduler();
}

// Информация о задании по клику в списке
void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    ui->gridWidgetTaskInfo->show();

    selectedTaskName = item->text();
    selectedTask = configData.getTask(selectedTaskName);

    if(selectedTask.contains("type")) ui->labelType->setText(QString::fromStdString(selectedTask["type"]));
    else ui->labelType->setText("-");

    if(selectedTask.contains("sourcePath"))
    {
        QString path = QString::fromStdString(selectedTask["sourcePath"]);
        if(path.size() > pathMaxLenght)
        {
            path = extras::cutPath(QString::fromStdString(selectedTask["sourcePath"]), pathMaxLenght);
        }
        ui->labelSourcePath->setText(path);
    }
    else ui->labelSourcePath->setText("-");

    if(selectedTask.contains("destPath"))
    {
        QString path = QString::fromStdString(selectedTask["destPath"]);
        if(path.size() > pathMaxLenght)
        {
            path = extras::cutPath(QString::fromStdString(selectedTask["destPath"]), pathMaxLenght);
        }
        ui->labelDestPath->setText(path);
    }
    else ui->labelDestPath->setText("-");

    if(selectedTask.contains("copiesNum"))
    {
        int copiesNum = selectedTask["copiesNum"];
        ui->labelCopiesNum->setText(QString::number(copiesNum));
    }
    else ui->labelCopiesNum->setText("-");

    if(selectedTask.contains("lastStartTime"))ui->labelLastRun->setText(QString::fromStdString(selectedTask["lastStartTime"]));
    else ui->labelLastRun->setText("-");

    if(selectedTask.contains("lastCompliteTime"))ui->labelLastComplite->setText(QString::fromStdString(selectedTask["lastCompliteTime"]));
    else ui->labelLastComplite->setText("-");

    if(selectedTask.contains("execTime"))ui->labelExecTime->setText(QString::fromStdString(selectedTask["execTime"]));
    else ui->labelExecTime->setText("-");

    ui->labelExecResult->setText("Статус последнего выполнения!"); // Успешно, с ошибками, отменено пользователем

    if(selectedTask.contains("repeat"))
    {
        if(selectedTask["repeat"] == "daily")
        {
            ui->labelNextRun->setText("daily");
        }
        if(selectedTask["repeat"] == "monthly")
        {
            ui->labelNextRun->setText("monthly");
        }
        if(selectedTask["repeat"] == "weekly")
        {
            ui->labelNextRun->setText("weekly");
        }
        if(selectedTask["repeat"] == "once")
        {
            QDateTime dateTime;
            dateTime.setDate(QDate::fromString(QString::fromStdString(selectedTask["date"]), "dd.MM.yyyy"));
            dateTime.setTime(QTime::fromString(QString::fromStdString(selectedTask["time"]), "hh:mm"));
            ui->labelNextRun->setText(dateTime.toString("dd.MM.yyyy hh:mm"));
        }
    }
    else ui->labelNextRun->setText("-");

    if(selectedTask.contains("currentCopiesNum"))
    {
        int num = selectedTask["currentCopiesNum"];
        ui->labelCurrentCopiesNum->setText(QString::number(num));
    }
    else ui->labelCurrentCopiesNum->setText("-");

    showTaskLog();
}
