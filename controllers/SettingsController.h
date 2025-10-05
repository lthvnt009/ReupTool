// Vị trí: /src/controllers/SettingsController.h
// Phiên bản: 1.4 (Hoàn thiện Giai đoạn 4)
// Mô tả: Controller chính cho toàn bộ Tab Cài đặt.

#pragma once

#include <QObject>
#include <QFont>
#include <QList>
#include "../models.h"

// Forward declarations
class SettingsWidget;
class SettingsService;

class SettingsController : public QObject
{
    Q_OBJECT
public:
    explicit SettingsController(SettingsWidget* widget, QObject *parent = nullptr);
    ~SettingsController();

signals:
    void languageChanged(const QString& language);
    void themeChanged(const QString& theme);
    void fontChanged(const QFont& font);
    void restartRequired();

private slots:
    // Slots cho các tab
    void onFontChangeRequested();
    void onChangeDbPath();
    void onBackup();
    void onRestore();
    void onAddApiKey();
    void onEditApiKey();
    void onDeleteApiKey();
    void onAddProxy();
    void onEditProxy();
    void onDeleteProxy();
    void onImportCookieFile();

private:
    void loadInitialSettings();
    void setupConnections();
    void saveApiKeys(); // Helper
    void saveProxies(); // Helper

    SettingsWidget* m_widget;
    SettingsService* m_settingsService;

    // State
    QList<ApiKey> m_apiKeys;
    QList<Proxy> m_proxies;
};

