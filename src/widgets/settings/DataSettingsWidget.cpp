// Vị trí: src/widgets/settings/DataSettingsWidget.cpp
// Phiên bản: 1.1 (Thêm signals và methods)
#include "datasettingswidget.h"
#include "ui_datasettingswidget.h"

DataSettingsWidget::DataSettingsWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::DataSettingsWidget) {
    ui->setupUi(this);

    connect(ui->changeDbPathButton, &QPushButton::clicked, this, &DataSettingsWidget::changeDbPathClicked);
    connect(ui->backupButton, &QPushButton::clicked, this, &DataSettingsWidget::backupClicked);
    connect(ui->restoreButton, &QPushButton::clicked, this, &DataSettingsWidget::restoreClicked);
    connect(ui->autoSaveCheckBox, &QCheckBox::toggled, this, &DataSettingsWidget::autoSaveToggled);
    connect(ui->autoSaveIntervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &DataSettingsWidget::autoSaveIntervalChanged);

    // Kích hoạt spinbox chỉ khi checkbox được chọn
    connect(ui->autoSaveCheckBox, &QCheckBox::toggled, ui->autoSaveIntervalSpinBox, &QSpinBox::setEnabled);
    ui->autoSaveIntervalSpinBox->setEnabled(ui->autoSaveCheckBox->isChecked());
}

DataSettingsWidget::~DataSettingsWidget() {
    delete ui;
}

void DataSettingsWidget::setDbPath(const QString &path)
{
    ui->dbPathLineEdit->setText(path);
}

void DataSettingsWidget::setAutoSaveEnabled(bool enabled)
{
    ui->autoSaveCheckBox->setChecked(enabled);
}

void DataSettingsWidget::setAutoSaveInterval(int minutes)
{
    ui->autoSaveIntervalSpinBox->setValue(minutes);
}
