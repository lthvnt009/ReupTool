// Vị trí: src/widgets/settings/UpdateSettingsWidget.cpp
// Phiên bản: 1.1 (Thêm logic Cập nhật)

#include "updatesettingswidget.h"
#include "ui_updatesettingswidget.h"
#include "../../appsettings.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QProcess>
#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QDesktopServices>

// Phiên bản hiện tại của ứng dụng, nên được định nghĩa ở một nơi tập trung
const QString APP_VERSION = "1.3.6"; 

UpdateSettingsWidget::UpdateSettingsWidget(QWidget *parent) :
    QWidget(parent), 
    ui(new Ui::UpdateSettingsWidget),
    m_networkManager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);
    ui->updateProgressBar->setVisible(false);
    ui->aboutLabel->setText(QString("ReupTool phiên bản %1\nBản quyền © 2024 TMTSoftware").arg(APP_VERSION));

    // Kết nối cho tab Cập nhật
    connect(m_networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply){
        if (!reply) return;
        // Phân loại reply dựa trên URL
        if(reply->request().url().path().contains("yt-dlp/yt-dlp")) {
            onYtdlpUpdateCheckFinished(reply);
        } else { // Giả định là của ReupTool
            onAppUpdateCheckFinished(reply);
        }
    });

    connect(ui->checkUpdateButton, &QPushButton::clicked, this, &UpdateSettingsWidget::checkForUpdates);
    connect(ui->updateAppButton, &QPushButton::clicked, this, &UpdateSettingsWidget::onUpdateAppClicked);
    connect(ui->updateYtdlpButton, &QPushButton::clicked, this, &UpdateSettingsWidget::onUpdateYtdlpClicked);
    connect(ui->updateFfmpegButton, &QPushButton::clicked, this, &UpdateSettingsWidget::onUpdateFfmpegClicked);

    // Kết nối để lưu cài đặt
    connect(ui->autoCheckUpdateCheckBox, &QCheckBox::toggled, this, &UpdateSettingsWidget::saveSettings);
}

UpdateSettingsWidget::~UpdateSettingsWidget() {
    delete ui;
}

void UpdateSettingsWidget::loadSettings()
{
    QSettings& settings = getAppSettings();
    ui->autoCheckUpdateCheckBox->setChecked(settings.value("Update/autoCheckOnStartup", true).toBool());
}

void UpdateSettingsWidget::saveSettings()
{
    QSettings& settings = getAppSettings();
    settings.setValue("Update/autoCheckOnStartup", ui->autoCheckUpdateCheckBox->isChecked());
}

void UpdateSettingsWidget::checkForUpdates()
{
    ui->currentAppVersionLabel->setText(APP_VERSION);
    
    // Kiểm tra phiên bản ReupTool
    ui->latestAppVersionLabel->setText("Đang kiểm tra phiên bản ReupTool...");
    m_networkManager->get(QNetworkRequest(QUrl("https://api.github.com/repos/tmtsoftware/reuptool/releases/latest")));
    
    // Kiểm tra phiên bản yt-dlp
    ui->ytdlpVersionLabel->setText("Đang kiểm tra phiên bản yt-dlp...");
    m_networkManager->get(QNetworkRequest(QUrl("https://api.github.com/repos/yt-dlp/yt-dlp/releases/latest")));

    // Lấy phiên bản yt-dlp cục bộ
    QProcess* ytdlpProcess = new QProcess(this);
    connect(ytdlpProcess, &QProcess::finished, this, [this, ytdlpProcess](int exitCode, QProcess::ExitStatus exitStatus){
        QString currentVersionText = "Hiện tại: ";
        if(exitStatus == QProcess::NormalExit && exitCode == 0){
            currentVersionText += ytdlpProcess->readAllStandardOutput().trimmed();
        } else {
            currentVersionText += "không thể kiểm tra";
        }
        
        QString fullText = ui->ytdlpVersionLabel->text();
        if(fullText.contains("|")) {
            fullText = currentVersionText + " " + fullText.section('|', 1);
        } else {
            fullText = currentVersionText;
        }
        ui->ytdlpVersionLabel->setText(fullText);

        ytdlpProcess->deleteLater();
    });
    
    QSettings& settings = getAppSettings();
    QString ytdlpPath = settings.value("DownloadSettings/ytdlpPath", "lib/yt-dlp.exe").toString();
    ytdlpProcess->start(ytdlpPath, {"--version"});
}

void UpdateSettingsWidget::onAppUpdateCheckFinished(QNetworkReply* reply)
{
    if(reply->error() == QNetworkReply::NoError){
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject release = doc.object();
        QString latestVersion = release["tag_name"].toString().remove('v');
        
        ui->latestAppVersionLabel->setText("Mới nhất: " + latestVersion);
        if(latestVersion > APP_VERSION){
            ui->updateAppButton->setEnabled(true);
            ui->updateAppButton->setText("Cập nhật lên " + latestVersion);
            ui->appChangelogBrowser->setMarkdown(release["body"].toString());
            
            QJsonArray assets = release["assets"].toArray();
            if(!assets.isEmpty()){
                for(const auto& assetValue : assets){
                    QJsonObject assetObj = assetValue.toObject();
                    if(assetObj["name"].toString().endsWith(".zip")){
                         m_appUpdateUrl = assetObj["browser_download_url"].toString();
                         break;
                    }
                }
            }
        } else {
            ui->latestAppVersionLabel->setText(ui->latestAppVersionLabel->text() + " (Bạn đang dùng phiên bản mới nhất)");
            ui->appChangelogBrowser->setText("Không có thay đổi mới.");
        }
    } else {
        ui->latestAppVersionLabel->setText("Lỗi kiểm tra cập nhật: " + reply->errorString());
    }
    reply->deleteLater();
}

void UpdateSettingsWidget::onYtdlpUpdateCheckFinished(QNetworkReply* reply)
{
    QString currentText = ui->ytdlpVersionLabel->text();
    if(reply->error() == QNetworkReply::NoError){
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject release = doc.object();
        QString latestVersion = release["tag_name"].toString();
        
        currentText += " | Mới nhất: " + latestVersion;
        ui->ytdlpVersionLabel->setText(currentText);

        if(!currentText.contains(latestVersion)){
            ui->updateYtdlpButton->setEnabled(true);
            
            QJsonArray assets = release["assets"].toArray();
            for(const auto& assetValue : assets){
                QJsonObject assetObj = assetValue.toObject();
                if(assetObj["name"].toString() == "yt-dlp.exe"){
                     m_ytdlpUpdateUrl = assetObj["browser_download_url"].toString();
                     break;
                }
            }
        }
    } else {
        ui->ytdlpVersionLabel->setText(currentText + " | Lỗi kiểm tra");
    }
    reply->deleteLater();
}

void UpdateSettingsWidget::onUpdateAppClicked()
{
    if(m_appUpdateUrl.isEmpty()) return;
    QString savePath = QCoreApplication::applicationDirPath() + "/update.zip";
    startDownload(QUrl(m_appUpdateUrl), savePath);
    ui->updateStatusLabel->setText("Đang tải bản cập nhật ứng dụng...");
}

void UpdateSettingsWidget::onUpdateYtdlpClicked()
{
    if(m_ytdlpUpdateUrl.isEmpty()) return;
    QSettings& settings = getAppSettings();
    QString savePath = settings.value("DownloadSettings/ytdlpPath", "lib/yt-dlp.exe").toString();
     if (savePath.isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng chỉ định đường dẫn đến yt-dlp.exe trong Cài đặt Tải về.");
        return;
    }
    startDownload(QUrl(m_ytdlpUpdateUrl), savePath);
    ui->updateStatusLabel->setText("Đang tải yt-dlp.exe...");
}

void UpdateSettingsWidget::onUpdateFfmpegClicked()
{
    QMessageBox::information(this, "Thông báo", "Chức năng cập nhật FFMPEG tự động sẽ được phát triển trong tương lai.");
}


void UpdateSettingsWidget::startDownload(const QUrl& url, const QString& savePath)
{
    if (m_currentDownload) {
        QMessageBox::information(this, "Thông báo", "Một quá trình tải về khác đang diễn ra.");
        return;
    }

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
    m_currentDownload = m_networkManager->get(request);

    connect(m_currentDownload, &QNetworkReply::downloadProgress, this, &UpdateSettingsWidget::onDownloadProgress);
    connect(m_currentDownload, &QNetworkReply::finished, this, &UpdateSettingsWidget::onUpdateDownloadFinished);
    
    m_downloadedFile = new QFile(savePath, this);
    if (!m_downloadedFile->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Lỗi", "Không thể ghi file: " + savePath);
        m_currentDownload->abort();
        delete m_downloadedFile;
        m_downloadedFile = nullptr;
        return;
    }
    connect(m_currentDownload, &QNetworkReply::readyRead, this, [this](){
        if(m_downloadedFile) m_downloadedFile->write(m_currentDownload->readAll());
    });
    
    ui->updateProgressBar->setValue(0);
    ui->updateProgressBar->setVisible(true);
}

void UpdateSettingsWidget::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if(bytesTotal > 0){
        ui->updateProgressBar->setValue( (bytesReceived * 100) / bytesTotal );
    }
}

void UpdateSettingsWidget::onUpdateDownloadFinished()
{
    if (m_downloadedFile) {
        m_downloadedFile->close();
        delete m_downloadedFile;
        m_downloadedFile = nullptr;
    }

    if(m_currentDownload->error() == QNetworkReply::NoError){
        ui->updateStatusLabel->setText("Tải về hoàn tất! Vui lòng khởi động lại ứng dụng hoặc chạy lại kiểm tra phiên bản.");
        if (m_currentDownload->request().url().toString().contains("update.zip")) {
             QMessageBox::information(this, "Hoàn tất", "Đã tải xong file cập nhật. Vui lòng giải nén file update.zip và chép đè vào thư mục ứng dụng.");
             QDesktopServices::openUrl(QUrl::fromLocalFile(QCoreApplication::applicationDirPath()));
        } else {
             QMessageBox::information(this, "Hoàn tất", "Đã cập nhật công cụ thành công.");
             checkForUpdates(); // Kiểm tra lại phiên bản sau khi cập nhật
        }
    } else {
        ui->updateStatusLabel->setText("Lỗi tải về: " + m_currentDownload->errorString());
    }
    m_currentDownload->deleteLater();
    m_currentDownload = nullptr;
    ui->updateProgressBar->setVisible(false);
}
