// Vị trí: /src/dialogs/datainputsettingsdialog.cpp
// Phiên bản: 2.2 (Loại bỏ tùy chọn thừa)

#include "datainputsettingsdialog.h"
#include "ui_datainputsettingsdialog.h"
#include "../appsettings.h"
#include <QSettings>

DataInputSettingsDialog::DataInputSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DataInputSettingsDialog)
{
    ui->setupUi(this);
    loadSettings();
}

DataInputSettingsDialog::~DataInputSettingsDialog()
{
    delete ui;
}

void DataInputSettingsDialog::loadSettings()
{
    QSettings& settings = getAppSettings();
    settings.beginGroup("DataInput");
    ui->warnOnUnfinishedCheckBox->setChecked(settings.value("warnOnUnfinished", true).toBool());
    ui->moveDownOnSaveCheckBox->setChecked(settings.value("moveDownOnSave", false).toBool());
    settings.endGroup();
}

void DataInputSettingsDialog::saveSettings()
{
    QSettings& settings = getAppSettings();
    settings.beginGroup("DataInput");
    settings.setValue("warnOnUnfinished", ui->warnOnUnfinishedCheckBox->isChecked());
    settings.setValue("moveDownOnSave", ui->moveDownOnSaveCheckBox->isChecked());
    settings.endGroup();
}

void DataInputSettingsDialog::accept()
{
    saveSettings();
    QDialog::accept();
}
