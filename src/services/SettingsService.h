// Vị trí: /src/services/SettingsService.h
// Phiên bản: 1.3 (Thêm Nguồn Dữ liệu, Mạng)
// Mô tả: Service trung tâm để quản lý việc đọc và ghi tất cả cài đặt vào file config.ini.

#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFont>
#include "../models.h" // Thêm

class SettingsService : public QObject
{
    Q_OBJECT
public:
    explicit SettingsService(QObject *parent = nullptr);

    // Cài đặt chung
    void saveLanguage(const QString& language);
    QString loadLanguage() const;
    void saveRememberWindowState(bool enabled);
    bool loadRememberWindowState() const;
    void saveWindowGeometry(const QByteArray& geometry);
    QByteArray loadWindowGeometry() const;
    void saveDatabasePath(const QString& path);
    QString loadDatabasePath() const;

    // Cài đặt giao diện
    void saveTheme(const QString& theme);
    QString loadTheme() const;
    void saveAppFont(const QFont& font);
    QFont loadAppFont() const;

    // Cài đặt Dữ liệu & Nhập liệu
    void saveAutoSave(bool enabled);
    bool loadAutoSave() const;
    void saveAutoSaveInterval(int minutes);
    int loadAutoSaveInterval() const;

    // Cài đặt Nguồn Dữ liệu
    void saveFetchStrategy(const QString& strategy);
    QString loadFetchStrategy() const;
    void saveApiKeys(const QList<ApiKey>& keys);
    QList<ApiKey> loadApiKeys() const;

    // Cài đặt Mạng
    void saveProxies(const QList<Proxy>& proxies);
    QList<Proxy> loadProxies() const;
    void saveCookieFile(const QString& cookieContent);
    QString loadCookieFile() const;


signals:
    void languageChanged(const QString& language);
    void themeChanged(const QString& theme);

};

