// Vị trí: src/widgets/settings/GeneralSettingsWidget.cpp
// Phiên bản: 1.7 (Sửa lỗi Build)

#include "generalsettingswidget.h"
#include "ui_generalsettingswidget.h"

GeneralSettingsWidget::GeneralSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GeneralSettingsWidget)
{
    ui->setupUi(this);

    // Kết nối tín hiệu từ UI để phát ra ngoài
    connect(ui->languageComboBox, &QComboBox::currentTextChanged, this, &GeneralSettingsWidget::languageChanged);
    
    // SỬA LỖI: Tên đúng của checkbox trong file .ui là 'rememberWindowSizeCheckBox'
    connect(ui->rememberWindowSizeCheckBox, &QCheckBox::toggled, this, &GeneralSettingsWidget::rememberWindowStateChanged);
}

GeneralSettingsWidget::~GeneralSettingsWidget()
{
    delete ui;
}

void GeneralSettingsWidget::setLanguage(const QString &language)
{
    // Tạm khóa tín hiệu để tránh phát lại tín hiệu khi đang cài đặt
    ui->languageComboBox->blockSignals(true);
    ui->languageComboBox->setCurrentText(language);
    ui->languageComboBox->blockSignals(false);
}

void GeneralSettingsWidget::setRememberWindowState(bool enabled)
{
    // SỬA LỖI: Tên đúng của checkbox trong file .ui là 'rememberWindowSizeCheckBox'
    ui->rememberWindowSizeCheckBox->blockSignals(true);
    ui->rememberWindowSizeCheckBox->setChecked(enabled);
    ui->rememberWindowSizeCheckBox->blockSignals(false);
}
