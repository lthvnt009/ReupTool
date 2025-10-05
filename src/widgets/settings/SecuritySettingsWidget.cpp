// Vị trí: src/widgets/settings/SecuritySettingsWidget.cpp
// Phiên bản: 1.1 (Hoàn thiện)
#include "securitysettingswidget.h"
#include "ui_securitysettingswidget.h"

SecuritySettingsWidget::SecuritySettingsWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::SecuritySettingsWidget) {
    ui->setupUi(this);

    // Tạm thời vô hiệu hóa
    ui->setDbPasswordButton->setEnabled(false);
    ui->setDbPasswordButton->setToolTip("Tính năng sẽ được phát triển trong phiên bản tương lai.");

    connect(ui->setDbPasswordButton, &QPushButton::clicked, this, &SecuritySettingsWidget::setDbPasswordClicked);
}

SecuritySettingsWidget::~SecuritySettingsWidget() {
    delete ui;
}
