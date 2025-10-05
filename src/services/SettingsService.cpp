// Vị trí: /src/services/SettingsService.cpp
// Phiên bản: 1.3 (Thêm Nguồn Dữ liệu, Mạng)

#include "settingsservice.h"
#include "../appsettings.h"
#include <QSettings>
#include <QFontDatabase>
#include <QDataStream> // Thêm

SettingsService::SettingsService(QObject *parent)
    : QObject{parent}
{}

// --- Cài đặt chung ---

void SettingsService::saveLanguage(const QString &language)
{
    getAppSettings().setValue("General/language", language);
    emit languageChanged(language);
}

QString SettingsService::loadLanguage() const
{
    return getAppSettings().value("General/language", "Tiếng Việt").toString();
}

void SettingsService::saveRememberWindowState(bool enabled)
{
    getAppSettings().setValue("General/rememberWindowState", enabled);
}

bool SettingsService::loadRememberWindowState() const
{
    return getAppSettings().value("General/rememberWindowState", true).toBool();
}

void SettingsService::saveWindowGeometry(const QByteArray &geometry)
{
    if (loadRememberWindowState()) {
        getAppSettings().setValue("General/windowGeometry", geometry);
    }
}

QByteArray SettingsService::loadWindowGeometry() const
{
    if (loadRememberWindowState()) {
        return getAppSettings().value("General/windowGeometry").toByteArray();
    }
    return QByteArray();
}

void SettingsService::saveDatabasePath(const QString &path)
{
    getAppSettings().setValue("databasePath", path);
}

QString SettingsService::loadDatabasePath() const
{
    return getAppSettings().value("databasePath").toString();
}


// --- Cài đặt giao diện ---

void SettingsService::saveTheme(const QString &theme)
{
    getAppSettings().setValue("Appearance/theme", theme);
    emit themeChanged(theme);
}

QString SettingsService::loadTheme() const
{
    return getAppSettings().value("Appearance/theme", "System").toString();
}

void SettingsService::saveAppFont(const QFont &font)
{
    getAppSettings().setValue("Appearance/font", font.toString());
}

QFont SettingsService::loadAppFont() const
{
    QString fontString = getAppSettings().value("Appearance/font").toString();
    QFont font;
    if (!fontString.isEmpty()) {
        font.fromString(fontString);
    } else {
        font = QFont(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    }
    return font;
}

// --- Cài đặt Dữ liệu & Nhập liệu ---

void SettingsService::saveAutoSave(bool enabled)
{
    getAppSettings().setValue("DataInput/autoSave", enabled);
}

bool SettingsService::loadAutoSave() const
{
    return getAppSettings().value("DataInput/autoSave", false).toBool();
}

void SettingsService::saveAutoSaveInterval(int minutes)
{
    getAppSettings().setValue("DataInput/autoSaveInterval", minutes);
}

int SettingsService::loadAutoSaveInterval() const
{
    return getAppSettings().value("DataInput/autoSaveInterval", 5).toInt();
}

// --- Cài đặt Nguồn Dữ liệu ---

void SettingsService::saveFetchStrategy(const QString &strategy)
{
    getAppSettings().setValue("DataSource/fetchStrategy", strategy);
}

QString SettingsService::loadFetchStrategy() const
{
    return getAppSettings().value("DataSource/fetchStrategy", "API").toString();
}

void SettingsService::saveApiKeys(const QList<ApiKey> &keys)
{
    QSettings& settings = getAppSettings();
    settings.beginWriteArray("ApiKeys");
    for (int i = 0; i < keys.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("id", keys[i].id);
        settings.setValue("alias", keys[i].alias);
        settings.setValue("keyValue", keys[i].keyValue); // NOTE: Cần mã hóa ở đây
        settings.setValue("isValid", keys[i].isValid);
    }
    settings.endArray();
}

QList<ApiKey> SettingsService::loadApiKeys() const
{
    QList<ApiKey> keys;
    QSettings& settings = getAppSettings();
    int size = settings.beginReadArray("ApiKeys");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ApiKey key;
        key.id = settings.value("id").toString();
        key.alias = settings.value("alias").toString();
        key.keyValue = settings.value("keyValue").toString(); // NOTE: Cần giải mã ở đây
        key.isValid = settings.value("isValid").toBool();
        keys.append(key);
    }
    settings.endArray();
    return keys;
}

// --- Cài đặt Mạng ---

void SettingsService::saveProxies(const QList<Proxy> &proxies)
{
    QSettings& settings = getAppSettings();
    settings.beginWriteArray("Proxies");
    for (int i = 0; i < proxies.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("id", proxies[i].id);
        settings.setValue("host", proxies[i].host);
        settings.setValue("port", proxies[i].port);
        settings.setValue("username", proxies[i].username);
        settings.setValue("password", proxies[i].password); // NOTE: Cần mã hóa
    }
    settings.endArray();
}

QList<Proxy> SettingsService::loadProxies() const
{
    QList<Proxy> proxies;
    QSettings& settings = getAppSettings();
    int size = settings.beginReadArray("Proxies");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        Proxy p;
        p.id = settings.value("id").toString();
        p.host = settings.value("host").toString();
        p.port = settings.value("port").toInt();
        p.username = settings.value("username").toString();
        p.password = settings.value("password").toString(); // NOTE: Cần giải mã
        proxies.append(p);
    }
    settings.endArray();
    return proxies;
}

void SettingsService::saveCookieFile(const QString &cookieContent)
{
    // NOTE: Cần mã hóa cookieContent trước khi lưu
    getAppSettings().setValue("Network/cookieData", cookieContent);
}

QString SettingsService::loadCookieFile() const
{
    // NOTE: Cần giải mã dữ liệu sau khi tải
    return getAppSettings().value("Network/cookieData").toString();
}

