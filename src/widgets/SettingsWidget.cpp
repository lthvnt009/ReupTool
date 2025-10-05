// Vị trí: /src/widgets/SettingsWidget.cpp
// Phiên bản: 1.3 (Sửa lỗi Build - Bổ sung getters)

#include "settingswidget.h"
#include "ui_settingswidget.h"

// Include header cho tất cả các widget tab con
#include "settings/generalsettingswidget.h"
#include "settings/appearancesettingswidget.h"
#include "settings/datasettingswidget.h"
#include "settings/DataSourceWidget.h"
#include "settings/networksettingswidget.h"
#include "settings/securitysettingswidget.h"
#include "settings/updatesettingswidget.h"

SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWidget)
{
    ui->setupUi(this);
}

SettingsWidget::~SettingsWidget()
{
    delete ui;
}

void SettingsWidget::setupTabs()
{
    // Khởi tạo các widget cho từng tab
    m_generalSettings = new GeneralSettingsWidget(this);
    m_appearanceSettings = new AppearanceSettingsWidget(this);
    m_dataSettings = new DataSettingsWidget(this);
    m_dataSourceSettings = new DataSourceWidget(this);
    m_networkSettings = new NetworkSettingsWidget(this);
    m_securitySettings = new SecuritySettingsWidget(this);
    m_updateSettings = new UpdateSettingsWidget(this);

    // Thêm các widget vào QTabWidget
    ui->tabWidget->addTab(m_generalSettings, "Chung");
    ui->tabWidget->addTab(m_appearanceSettings, "Giao diện");
    ui->tabWidget->addTab(m_dataSettings, "Dữ liệu & Sao lưu");
    ui->tabWidget->addTab(m_dataSourceSettings, "Nguồn Dữ liệu");
    ui->tabWidget->addTab(m_networkSettings, "Mạng");
    ui->tabWidget->addTab(m_securitySettings, "Bảo mật");
    ui->tabWidget->addTab(m_updateSettings, "Cập nhật & Giới thiệu");
}

GeneralSettingsWidget* SettingsWidget::getGeneralSettingsWidget() const { return m_generalSettings; }
AppearanceSettingsWidget* SettingsWidget::getAppearanceSettingsWidget() const { return m_appearanceSettings; }
DataSettingsWidget* SettingsWidget::getDataSettingsWidget() const { return m_dataSettings; }
DataSourceWidget* SettingsWidget::getDataSourceWidget() const { return m_dataSourceSettings; }
NetworkSettingsWidget* SettingsWidget::getNetworkSettingsWidget() const { return m_networkSettings; }
SecuritySettingsWidget* SettingsWidget::getSecuritySettingsWidget() const { return m_securitySettings; }
UpdateSettingsWidget* SettingsWidget::getUpdateSettingsWidget() const { return m_updateSettings; }

