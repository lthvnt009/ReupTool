// Vị trí: /src/widgets/SettingsWidget.h
// Phiên bản: 1.2 (Sửa lỗi Build - Bổ sung getters)
// Mô tả: Widget chính chứa QTabWidget cho Tab Cài đặt.

#pragma once

#include <QWidget>

// Forward declarations
namespace Ui { class SettingsWidget; }
class GeneralSettingsWidget;
class AppearanceSettingsWidget;
class DataSettingsWidget;
class DataSourceWidget;
class NetworkSettingsWidget;
class SecuritySettingsWidget;
class UpdateSettingsWidget;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    void setupTabs();

    // Getters cho các widget con
    GeneralSettingsWidget* getGeneralSettingsWidget() const;
    AppearanceSettingsWidget* getAppearanceSettingsWidget() const;
    DataSettingsWidget* getDataSettingsWidget() const;
    DataSourceWidget* getDataSourceWidget() const;
    NetworkSettingsWidget* getNetworkSettingsWidget() const;
    SecuritySettingsWidget* getSecuritySettingsWidget() const;
    UpdateSettingsWidget* getUpdateSettingsWidget() const;


private:
    Ui::SettingsWidget *ui;

    // Thành viên cho các widget tab con
    GeneralSettingsWidget* m_generalSettings;
    AppearanceSettingsWidget* m_appearanceSettings;
    DataSettingsWidget* m_dataSettings;
    DataSourceWidget* m_dataSourceSettings;
    NetworkSettingsWidget* m_networkSettings;
    SecuritySettingsWidget* m_securitySettings;
    UpdateSettingsWidget* m_updateSettings;
};

