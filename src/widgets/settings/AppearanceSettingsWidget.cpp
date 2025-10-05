// Vị trí: src/widgets/settings/AppearanceSettingsWidget.cpp
// Phiên bản: 1.6 (Sửa lỗi Build)

#include "appearancesettingswidget.h"
#include "ui_appearancesettingswidget.h"
#include <QButtonGroup>
#include <QFontDialog>
#include <QPushButton>

AppearanceSettingsWidget::AppearanceSettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AppearanceSettingsWidget)
{
    ui->setupUi(this);

    // Nhóm các radio button lại với nhau
    m_themeGroup = new QButtonGroup(this);
    m_themeGroup->addButton(ui->lightThemeRadioButton, 0);
    m_themeGroup->addButton(ui->darkThemeRadioButton, 1);
    m_themeGroup->addButton(ui->systemThemeRadioButton, 2);

    // Kết nối tín hiệu khi một nút trong nhóm được chọn
    connect(m_themeGroup, &QButtonGroup::buttonClicked, this, [this](QAbstractButton* button){
        if (button == ui->lightThemeRadioButton) {
            emit themeChanged("Light");
        } else if (button == ui->darkThemeRadioButton) {
            emit themeChanged("Dark");
        } else if (button == ui->systemThemeRadioButton) {
            emit themeChanged("System");
        }
    });

    // SỬA LỖI: Kết nối với QPushButton 'changeFontButton' đã được thêm vào file .ui
    connect(ui->changeFontButton, &QPushButton::clicked, this, &AppearanceSettingsWidget::fontChangeRequested);
}

AppearanceSettingsWidget::~AppearanceSettingsWidget()
{
    delete ui;
}

void AppearanceSettingsWidget::setTheme(const QString &theme)
{
    // Tạm khóa tín hiệu của group để tránh phát lại
    m_themeGroup->blockSignals(true);
    if (theme == "Light") {
        ui->lightThemeRadioButton->setChecked(true);
    } else if (theme == "Dark") {
        ui->darkThemeRadioButton->setChecked(true);
    } else { // Mặc định là System
        ui->systemThemeRadioButton->setChecked(true);
    }
    m_themeGroup->blockSignals(false);
}

void AppearanceSettingsWidget::setFont(const QFont &font)
{
    // SỬA LỖI: Cập nhật QLabel 'fontPreviewLabel' đã được thêm vào file .ui
    ui->fontPreviewLabel->setFont(font);
    ui->fontPreviewLabel->setText(QString("%1, %2pt").arg(font.family()).arg(font.pointSize()));
}
