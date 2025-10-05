// Vị trí: src/widgets/settings/UpdateSettingsWidget.h
// Phiên bản: 1.1 (Thêm logic Cập nhật)
#pragma once

#include <QWidget>

namespace Ui { class UpdateSettingsWidget; }

class QNetworkAccessManager;
class QNetworkReply;
class QFile;

class UpdateSettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit UpdateSettingsWidget(QWidget *parent = nullptr);
    ~UpdateSettingsWidget();

public slots:
    void checkForUpdates();
    void loadSettings();

signals:
    void autoCheckForUpdateChanged(bool enabled);

private slots:
    // Slots cho chức năng cập nhật
    void onAppUpdateCheckFinished(QNetworkReply* reply);
    void onYtdlpUpdateCheckFinished(QNetworkReply* reply);
    void onUpdateAppClicked();
    void onUpdateYtdlpClicked();
    void onUpdateFfmpegClicked();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdateDownloadFinished();
    void saveSettings();

private:
    void startDownload(const QUrl& url, const QString& savePath);
    
    Ui::UpdateSettingsWidget *ui;

    // Thành viên cho chức năng cập nhật
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentDownload = nullptr;
    QFile* m_downloadedFile = nullptr;
    QString m_appUpdateUrl;
    QString m_ytdlpUpdateUrl;
    QString m_ffmpegUpdateUrl;
};
