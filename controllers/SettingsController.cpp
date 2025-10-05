// Vị trí: /src/controllers/SettingsController.cpp
// Phiên bản: 1.6 (Sửa lỗi Build do sai tên file include)

#include "settingscontroller.h"
#include "../widgets/settingswidget.h"
#include "../services/settingsservice.h"
#include "../appsettings.h"

// Include các widget con và dialog
#include "../widgets/settings/generalsettingswidget.h"
#include "../widgets/settings/appearancesettingswidget.h"
#include "../widgets/settings/datasettingswidget.h"
#include "../widgets/settings/DataSourceWidget.h"
#include "../widgets/settings/networksettingswidget.h"
#include "../widgets/settings/securitysettingswidget.h"
#include "../widgets/settings/updatesettingswidget.h"
#include "../dialogs/apikeydialog.h"
#include "../dialogs/proxydialog.h"


#include <QFontDialog>
#include <QSettings>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

SettingsController::SettingsController(SettingsWidget* widget, QObject *parent)
    : QObject(parent)
    , m_widget(widget)
    , m_settingsService(new SettingsService(this))
{
    m_widget->setupTabs();
    loadInitialSettings();
    setupConnections();

    QSettings& settings = getAppSettings();
    if (settings.value("Update/autoCheckOnStartup", true).toBool()) {
        m_widget->getUpdateSettingsWidget()->checkForUpdates();
    }
}

SettingsController::~SettingsController()
{
    // Lưu lại các cài đặt khi controller bị hủy
    m_settingsService->saveFetchStrategy(m_widget->getDataSourceWidget()->getFetchStrategy());
    saveApiKeys();
    saveProxies();
}

void SettingsController::loadInitialSettings()
{
    // ... Nạp các tab cũ ...
    auto* generalWidget = m_widget->getGeneralSettingsWidget();
    generalWidget->setLanguage(m_settingsService->loadLanguage());
    generalWidget->setRememberWindowState(m_settingsService->loadRememberWindowState());

    auto* appearanceWidget = m_widget->getAppearanceSettingsWidget();
    appearanceWidget->setTheme(m_settingsService->loadTheme());
    appearanceWidget->setFont(m_settingsService->loadAppFont());

    auto* dataWidget = m_widget->getDataSettingsWidget();
    dataWidget->setDbPath(m_settingsService->loadDatabasePath());
    dataWidget->setAutoSaveEnabled(m_settingsService->loadAutoSave());
    dataWidget->setAutoSaveInterval(m_settingsService->loadAutoSaveInterval());

    // Nạp tab Nguồn Dữ liệu
    auto* dataSourceWidget = m_widget->getDataSourceWidget();
    dataSourceWidget->setFetchStrategy(m_settingsService->loadFetchStrategy());
    m_apiKeys = m_settingsService->loadApiKeys();
    dataSourceWidget->getApiKeyModel()->setApiKeys(m_apiKeys);

    // Nạp tab Mạng
    auto* networkWidget = m_widget->getNetworkSettingsWidget();
    m_proxies = m_settingsService->loadProxies();
    networkWidget->getProxyModel()->setProxies(m_proxies);
    if (!m_settingsService->loadCookieFile().isEmpty()) {
        networkWidget->setCookieStatus("Trạng thái: Đã nhập cookie.");
    }

    // Nạp tab Cập nhật
    auto* updateWidget = m_widget->getUpdateSettingsWidget();
    updateWidget->loadSettings();
}

void SettingsController::setupConnections()
{
    // ... Kết nối các tab cũ ...
    auto* generalWidget = m_widget->getGeneralSettingsWidget();
    connect(generalWidget, &GeneralSettingsWidget::languageChanged, m_settingsService, &SettingsService::saveLanguage);
    connect(generalWidget, &GeneralSettingsWidget::rememberWindowStateChanged, m_settingsService, &SettingsService::saveRememberWindowState);

    auto* appearanceWidget = m_widget->getAppearanceSettingsWidget();
    connect(appearanceWidget, &AppearanceSettingsWidget::themeChanged, m_settingsService, &SettingsService::saveTheme);
    connect(appearanceWidget, &AppearanceSettingsWidget::fontChangeRequested, this, &SettingsController::onFontChangeRequested);
    
    auto* dataWidget = m_widget->getDataSettingsWidget();
    connect(dataWidget, &DataSettingsWidget::changeDbPathClicked, this, &SettingsController::onChangeDbPath);
    connect(dataWidget, &DataSettingsWidget::backupClicked, this, &SettingsController::onBackup);
    connect(dataWidget, &DataSettingsWidget::restoreClicked, this, &SettingsController::onRestore);
    connect(dataWidget, &DataSettingsWidget::autoSaveToggled, m_settingsService, &SettingsService::saveAutoSave);
    connect(dataWidget, &DataSettingsWidget::autoSaveIntervalChanged, m_settingsService, &SettingsService::saveAutoSaveInterval);
    
    // --- Kết nối Tab Nguồn Dữ liệu ---
    auto* dataSourceWidget = m_widget->getDataSourceWidget();
    connect(dataSourceWidget, &DataSourceWidget::addApiKeyClicked, this, &SettingsController::onAddApiKey);
    connect(dataSourceWidget, &DataSourceWidget::editApiKeyClicked, this, &SettingsController::onEditApiKey);
    connect(dataSourceWidget, &DataSourceWidget::deleteApiKeyClicked, this, &SettingsController::onDeleteApiKey);

    // --- Kết nối Tab Mạng ---
    auto* networkWidget = m_widget->getNetworkSettingsWidget();
    connect(networkWidget, &NetworkSettingsWidget::addProxyClicked, this, &SettingsController::onAddProxy);
    connect(networkWidget, &NetworkSettingsWidget::editProxyClicked, this, &SettingsController::onEditProxy);
    connect(networkWidget, &NetworkSettingsWidget::deleteProxyClicked, this, &SettingsController::onDeleteProxy);
    connect(networkWidget, &NetworkSettingsWidget::importCookieFileClicked, this, &SettingsController::onImportCookieFile);
    
    // --- Chuyển tiếp tín hiệu từ Service ra ngoài ---
    connect(m_settingsService, &SettingsService::languageChanged, this, &SettingsController::languageChanged);
    connect(m_settingsService, &SettingsService::themeChanged, this, &SettingsController::themeChanged);
}

void SettingsController::onFontChangeRequested()
{
    auto* appearanceWidget = m_widget->getAppearanceSettingsWidget();
    bool ok;
    QFont currentFont = m_settingsService->loadAppFont();
    QFont newFont = QFontDialog::getFont(&ok, currentFont, m_widget, "Chọn Phông chữ");
    if (ok) {
        m_settingsService->saveAppFont(newFont);
        appearanceWidget->setFont(newFont);
        emit fontChanged(newFont);
    }
}

void SettingsController::onChangeDbPath()
{
    QString newDbPath = QFileDialog::getSaveFileName(m_widget, "Chọn vị trí CSDL mới", "reuptool.db", "Database Files (*.db)");
    if (newDbPath.isEmpty()) return;

    m_settingsService->saveDatabasePath(newDbPath);
    m_widget->getDataSettingsWidget()->setDbPath(newDbPath);
    QMessageBox::information(m_widget, "Hoàn tất", 
        "Đường dẫn cơ sở dữ liệu đã được cập nhật.\n\n"
        "Vui lòng **khởi động lại ứng dụng**.\n\n"
        "Lưu ý: Nếu bạn muốn sử dụng dữ liệu cũ, bạn cần **tự di chuyển file .db** của mình đến vị trí mới trước khi khởi động lại. Nếu không, một CSDL trống sẽ được tạo.");
    emit restartRequired();
}

void SettingsController::onBackup()
{
    QString backupPath = QFileDialog::getSaveFileName(m_widget, "Lưu bản sao lưu", "reuptool_backup.db", "Database Files (*.db)");
    if (backupPath.isEmpty()) return;
    
    QString currentDbPath = m_settingsService->loadDatabasePath();
    if (QFile::exists(backupPath)) QFile::remove(backupPath);

    if (QFile::copy(currentDbPath, backupPath)) {
        QMessageBox::information(m_widget, "Hoàn tất", QString("Đã sao lưu CSDL thành công vào:\n%1").arg(backupPath));
    } else {
        QMessageBox::critical(m_widget, "Lỗi", "Không thể tạo file sao lưu. Vui lòng kiểm tra lại quyền ghi.");
    }
}

void SettingsController::onRestore()
{
    QString backupPath = QFileDialog::getOpenFileName(m_widget, "Chọn file sao lưu để phục hồi", "", "Database Files (*.db)");
    if (backupPath.isEmpty()) return;

    auto reply = QMessageBox::warning(m_widget, "Xác nhận Phục hồi", 
        "Hành động này sẽ **GHI ĐÈ** toàn bộ dữ liệu hiện tại vào **lần khởi động tiếp theo** và **KHÔNG THỂ HOÀN TÁC**.\n\n"
        "Bạn có chắc chắn muốn tiếp tục?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (reply == QMessageBox::No) return;
    
    QSettings& settings = getAppSettings();
    settings.setValue("Update/restoreDbOnStartup", backupPath);
    
    QMessageBox::information(m_widget, "Hoàn tất", "Đã lên lịch phục hồi CSDL. Vui lòng khởi động lại ứng dụng để hoàn tất quá trình.");
    emit restartRequired();
}

void SettingsController::onAddApiKey()
{
    ApiKeyDialog dialog(m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        ApiKey newKey = dialog.getApiKey();
        if (newKey.keyValue.isEmpty()) return;
        m_apiKeys.append(newKey);
        m_widget->getDataSourceWidget()->getApiKeyModel()->setApiKeys(m_apiKeys);
    }
}

void SettingsController::onEditApiKey()
{
    auto* dataSourceWidget = m_widget->getDataSourceWidget();
    int index = dataSourceWidget->getSelectedApiKeyIndex();
    if (index < 0 || index >= m_apiKeys.size()) return;

    ApiKeyDialog dialog(m_apiKeys[index], m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        m_apiKeys[index] = dialog.getApiKey();
        dataSourceWidget->getApiKeyModel()->setApiKeys(m_apiKeys);
    }
}

void SettingsController::onDeleteApiKey()
{
    auto* dataSourceWidget = m_widget->getDataSourceWidget();
    int index = dataSourceWidget->getSelectedApiKeyIndex();
    if (index < 0 || index >= m_apiKeys.size()) return;

    if (QMessageBox::question(m_widget, "Xác nhận", "Bạn có chắc muốn xóa API key này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_apiKeys.removeAt(index);
        dataSourceWidget->getApiKeyModel()->setApiKeys(m_apiKeys);
    }
}

void SettingsController::onAddProxy()
{
    ProxyDialog dialog(m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        Proxy newProxy = dialog.getProxy();
        if (newProxy.host.isEmpty() || newProxy.port == 0) return;
        m_proxies.append(newProxy);
        m_widget->getNetworkSettingsWidget()->getProxyModel()->setProxies(m_proxies);
    }
}

void SettingsController::onEditProxy()
{
    auto* networkWidget = m_widget->getNetworkSettingsWidget();
    int index = networkWidget->getSelectedProxyIndex();
    if (index < 0 || index >= m_proxies.size()) return;

    ProxyDialog dialog(m_proxies[index], m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        m_proxies[index] = dialog.getProxy();
        networkWidget->getProxyModel()->setProxies(m_proxies);
    }
}

void SettingsController::onDeleteProxy()
{
    auto* networkWidget = m_widget->getNetworkSettingsWidget();
    int index = networkWidget->getSelectedProxyIndex();
    if (index < 0 || index >= m_proxies.size()) return;

    if (QMessageBox::question(m_widget, "Xác nhận", "Bạn có chắc muốn xóa proxy này?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_proxies.removeAt(index);
        networkWidget->getProxyModel()->setProxies(m_proxies);
    }
}

void SettingsController::onImportCookieFile()
{
    QString filePath = QFileDialog::getOpenFileName(m_widget, "Chọn file cookies.txt", "", "Text files (*.txt)");
    if (filePath.isEmpty()) return;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(m_widget, "Lỗi", "Không thể đọc file đã chọn.");
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    m_settingsService->saveCookieFile(content);
    m_widget->getNetworkSettingsWidget()->setCookieStatus("Trạng thái: Đã nhập cookie từ file.");
    QMessageBox::information(m_widget, "Hoàn tất", "Đã nhập cookie từ file thành công.");
}

void SettingsController::saveApiKeys()
{
    m_settingsService->saveApiKeys(m_apiKeys);
}

void SettingsController::saveProxies()
{
    m_settingsService->saveProxies(m_proxies);
}

