#include "taskseditwindow.h"
#include "ui_taskseditwindow.h"


void TasksEditWindow::closeEvent(QCloseEvent *event)
{
    emit showParentWindow();
    this->deleteLater();
}

TasksEditWindow::TasksEditWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TasksEditWindow)
{
    ui->setupUi(this);
    currentSize = QSize(400, 500);
    this->resize(currentSize);
    hideWidgets();

    // добавить текущее время в окно выбора времени
    ui->timeEdit->setDateTime(QDateTime::currentDateTime());
    ui->dateEdit->setDateTime(QDateTime::currentDateTime());
    ui->verticalWidgetSystem->hide();
}

TasksEditWindow::~TasksEditWindow()
{
    delete ui;
}

void TasksEditWindow::saveTask()
{
    if(ui->lineEditTaskName->text() != "")
    {
        task["type"] = ui->comboBoxType->currentText().toStdString();
        task["days"] = daysToJson();
        task["months"] = monthsToJson();
        task["time"] = ui->timeEdit->text().toStdString();
        task["date"] = ui->dateEdit->text().toStdString();
        data.addTask(ui->lineEditTaskName->text(), task);
        data.writeConfig();
        emit sendData(data);
    }
}

void TasksEditWindow::showTask(const QString _name)
{
    task = data.getTask(_name);

    ui->lineEditTaskName->setText(_name);
    ui->lineEditSourcePath->setText(QString::fromStdString(task["sourcePath"]));
    ui->lineEditDestPath->setText(QString::fromStdString(task["destPath"]));
    ui->textEditLog->setText(QString::fromStdString(task.dump(4)));
    if(task["repeat"] == "daily")
    {
        ui->radioButtonDaily->setChecked(true);
        ui->radioButtonDaily->clicked(true);
    }
    if(task["repeat"] == "monthly")
    {
        ui->radioButtonMonthly->setChecked(true);
        ui->radioButtonMonthly->clicked(true);
    }
    if(task["repeat"] == "weekly")
    {
        ui->radioButtonWeekly->setChecked(true);
        ui->radioButtonWeekly->clicked(true);
    }
    if(task["repeat"] == "once")
    {
        ui->radioButtonOnce->setChecked(true);
        ui->radioButtonOnce->clicked(true);
    }

    if(!task["time"].empty())
    {
        QTime taskTime;
        QString stringTime = QString::fromStdString(task["time"]);
        taskTime = QTime::fromString(stringTime, "hh:mm");
        ui->timeEdit->setTime(taskTime);
    }

    if(!task["date"].empty())
    {
        QDate taskDate;
        QString stringDate = QString::fromStdString(task["date"]);

        taskDate = QDate::fromString(stringDate, "dd.MM.yyyy");

        ui->dateEdit->setDate(taskDate);
    }

    if(!task["type"].empty())
    {
        if(task["type"] == "Копия") ui->comboBoxType->setCurrentText("Копия");
        if(task["type"] == "Зеркало") ui->comboBoxType->setCurrentText("Зеркало");
        if(task["type"] == "Очистка") ui->comboBoxType->setCurrentText("Очистка");
    }
    else
    {
        ui->comboBoxType->setCurrentText("Копия");
    }
}

void TasksEditWindow::hideWidgets()
{
    ui->widgetDays->hide();
    ui->widgetMonths->hide();
    ui->dateEdit->hide();
}

bool TasksEditWindow::daySelected()
{
    return ui->checkBoxMon->isChecked() ||
            ui->checkBoxTue->isChecked() ||
            ui->checkBoxWed->isChecked() ||
            ui->checkBoxThu->isChecked() ||
            ui->checkBoxFri->isChecked() ||
            ui->checkBoxSat->isChecked() ||
            ui->checkBoxSun->isChecked();
}

bool TasksEditWindow::monthSelected()
{
    return ui->checkBoxJan->isChecked() ||
            ui->checkBoxFeb->isChecked() ||
            ui->checkBoxMar->isChecked() ||
            ui->checkBoxApr->isChecked() ||
            ui->checkBoxMay->isChecked() ||
            ui->checkBoxJun->isChecked() ||
            ui->checkBoxJul->isChecked() ||
            ui->checkBoxAug->isChecked() ||
            ui->checkBoxSep->isChecked() ||
            ui->checkBoxOct->isChecked() ||
            ui->checkBoxNov->isChecked() ||
            ui->checkBoxDec->isChecked();
}

void TasksEditWindow::clearDays()
{
    ui->checkBoxMon->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxTue->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxFri->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxWed->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxThu->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxFri->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxSat->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxSun->setCheckState(Qt::CheckState::Unchecked);
}

void TasksEditWindow::clearMonths()
{
    ui->checkBoxJan->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxFeb->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxMar->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxApr->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxMay->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxJun->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxJul->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxAug->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxSep->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxOct->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxNov->setCheckState(Qt::CheckState::Unchecked);
    ui->checkBoxDec->setCheckState(Qt::CheckState::Unchecked);
}

json TasksEditWindow::daysToJson()
{
    json tempJson;
    if(ui->checkBoxMon->isChecked()) tempJson.push_back("monday");
    if(ui->checkBoxTue->isChecked()) tempJson.push_back("tuesday");
    if(ui->checkBoxWed->isChecked()) tempJson.push_back("wednesday");
    if(ui->checkBoxThu->isChecked()) tempJson.push_back("thursday");
    if(ui->checkBoxFri->isChecked()) tempJson.push_back("friday");
    if(ui->checkBoxSat->isChecked()) tempJson.push_back("saturday");
    if(ui->checkBoxSun->isChecked()) tempJson.push_back("sunday");
    return tempJson;
}

void TasksEditWindow::daysFromJson()
{
    for(auto it : task["days"])
    {
        if(it == "monday")ui->checkBoxMon->setChecked(true);
        if(it == "tuesday")ui->checkBoxTue->setChecked(true);
        if(it == "wednesday")ui->checkBoxWed->setChecked(true);
        if(it == "thursday")ui->checkBoxThu->setChecked(true);
        if(it == "friday")ui->checkBoxFri->setChecked(true);
        if(it == "saturday")ui->checkBoxSat->setChecked(true);
        if(it == "sunday")ui->checkBoxSun->setChecked(true);
    }
}

json TasksEditWindow::monthsToJson()
{
    json tempJson;
    json::array_t array;
    if(ui->checkBoxJan->isChecked()) array.push_back("january");
    if(ui->checkBoxFeb->isChecked()) array.push_back("february");
    if(ui->checkBoxMar->isChecked()) array.push_back("march");
    if(ui->checkBoxApr->isChecked()) array.push_back("april");
    if(ui->checkBoxMay->isChecked()) array.push_back("may");
    if(ui->checkBoxJun->isChecked()) array.push_back("june");
    if(ui->checkBoxJul->isChecked()) array.push_back("jule");
    if(ui->checkBoxAug->isChecked()) array.push_back("august");
    if(ui->checkBoxSep->isChecked()) array.push_back("september");
    if(ui->checkBoxOct->isChecked()) array.push_back("october");
    if(ui->checkBoxNov->isChecked()) array.push_back("november");
    if(ui->checkBoxDec->isChecked()) array.push_back("decembr");
    tempJson = array;
    return tempJson;
}

void TasksEditWindow::monthsFromJson()
{
    for(auto it : task["months"])
    {
        if(it == "january")ui->checkBoxJan->setChecked(true);
        if(it == "february")ui->checkBoxFeb->setChecked(true);
        if(it == "march")ui->checkBoxMar->setChecked(true);
        if(it == "april")ui->checkBoxApr->setChecked(true);
        if(it == "may")ui->checkBoxMay->setChecked(true);
        if(it == "june")ui->checkBoxJun->setChecked(true);
        if(it == "jule")ui->checkBoxJul->setChecked(true);
        if(it == "august")ui->checkBoxAug->setChecked(true);
        if(it == "september")ui->checkBoxSep->setChecked(true);
        if(it == "october")ui->checkBoxOct->setChecked(true);
        if(it == "november")ui->checkBoxNov->setChecked(true);
        if(it == "decembr")ui->checkBoxDec->setChecked(true);
    }
}

void TasksEditWindow::clearAllData()
{
    ui->lineEditSourcePath->clear();
    ui->lineEditDestPath->clear();
    ui->lineEditTaskName->clear();
    ui->textEditLog->clear();
    clearMonths();
    clearDays();
}

void TasksEditWindow::reciveData(JSONConverter _data)
{
    data = _data;
    lastPath = data.getConfig("lastPath");
    extras::showTasksList(ui->listWidget, data.getFullTasks());
}

void TasksEditWindow::on_pushButtonSourcePath_clicked()
{
    lastPath = QFileDialog::getExistingDirectory(this, "Выбрать папку откуда копируем", lastPath);

    ui->lineEditSourcePath->setText(lastPath);
    data.addConfig("lastPath", lastPath);
    task["sourcePath"] = lastPath.toStdString();
}


void TasksEditWindow::on_pushButtonDestPath_clicked()
{
    lastPath = QFileDialog::getExistingDirectory(this, "Выбрать папку куда копируем", lastPath);

    ui->lineEditDestPath->setText(lastPath);
    data.addConfig("lastPath", lastPath);
    task["destPath"] = lastPath.toStdString();
}

void TasksEditWindow::on_pushButtonSave_clicked()
{
    if(ui->lineEditTaskName->text() != "")
    {
        saveTask();
        extras::showTasksList(ui->listWidget, data.getFullTasks());
    }
}

void TasksEditWindow::on_radioButtonDaily_clicked()
{
    hideWidgets();
    task["repeat"] = "daily";
}

void TasksEditWindow::on_radioButtonWeekly_clicked()
{
    hideWidgets();
    ui->widgetDays->show();
    task["repeat"] = "weekly";
    daysFromJson();
}

void TasksEditWindow::on_radioButtonMonthly_clicked()
{
    hideWidgets();
    ui->widgetDays->show();
    ui->widgetMonths->show();
    task["repeat"] = "monthly";
    daysFromJson();
    monthsFromJson();
}

void TasksEditWindow::on_radioButtonOnce_clicked()
{
    hideWidgets();
    ui->dateEdit->show();
    task["repeat"] = "once";
}

void TasksEditWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    json tempJson = data.getFullTasks();
    ui->textEditLog->setText(QString::fromStdString(tempJson[item->text().toStdString()].dump(4)));
    clearDays();
    clearMonths();
    showTask(item->text());
}

void TasksEditWindow::on_pushButtonDeleteTask_clicked()
{
    if(ui->listWidget->currentItem() != NULL)
    {
        data.removeTask(ui->listWidget->currentItem()->text());
        data.writeConfig();
        emit sendData(data);
        ui->listWidget->clear();
        extras::showTasksList(ui->listWidget, data.getFullTasks());
        clearAllData();
    }
}

void TasksEditWindow::on_pushButtonSystem_clicked()
{
    if(system)
    {
        ui->verticalWidgetSystem->hide();
        system = false;
    }
    else
    {
        ui->verticalWidgetSystem->show();
        system = true;
    }
}
