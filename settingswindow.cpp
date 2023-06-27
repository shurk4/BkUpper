#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::closeEvent(QCloseEvent *event)
{
    this->deleteLater();
}

void SettingsWindow::reciveData(JSONConverter _data)
{
    data = _data;

    if(data.getConfigState("startWithOS")) ui->checkBoxStartWithOS->setChecked(true);
    if(data.getConfigState("startMinimized")) ui->checkBoxStartMinimized->setChecked(true);
    if(data.getConfigState("closeToTray")) ui->checkBoxMinimizeToTray->setChecked(true);
}

void SettingsWindow::on_checkBoxStartWithOS_stateChanged(int arg1)
{
    if(ui->checkBoxStartWithOS->isChecked())
    {
        data.addConfig("startWithOS", true);
    }
    else
    {
        data.addConfig("startWithOS", false);
    }

    emit sendData(data);
}

void SettingsWindow::on_checkBoxStartMinimized_stateChanged(int arg1)
{
    if(ui->checkBoxStartMinimized->isChecked())
    {
        data.addConfig("startMinimized", true);
    }
    else
    {
        data.addConfig("startMinimized", false);
    }

    emit sendData(data);
}

void SettingsWindow::on_checkBoxMinimizeToTray_stateChanged(int arg1)
{
    if(ui->checkBoxMinimizeToTray->isChecked())
    {
        data.addConfig("closeToTray", true);
    }
    else
    {
        data.addConfig("closeToTray", false);
    }

    emit sendData(data);
}

void SettingsWindow::on_pushButtonOk_clicked()
{
    this->close();
}
