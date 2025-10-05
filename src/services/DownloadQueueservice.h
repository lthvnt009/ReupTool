// Phiên bản: 1.2 (Sửa lỗi Build)
// Vị trí: /src/services/downloadqueueservice.h

#pragma once

#include <QObject>
#include <QList>
#include "../models.h"

// Forward declarations
class DownloadManager;
class TaskbarManager;

class DownloadQueueService : public QObject
{
    Q_OBJECT
public:
    explicit DownloadQueueService(QObject *parent = nullptr);
    ~DownloadQueueService();

    QList<DownloadItem>* getQueue();
    const DownloadItem& getItem(int index) const;

public slots:
    void setMaxConcurrentDownloads(int count);
    void addItem(const QString& url, const DownloadOptions& options);
    void addLegacyDownloads(const QList<Video>& videos, const DownloadOptions& options);
    void removeItem(int index);
    void skipItem(int index);
    void refreshItem(int index);
    void renameItem(int index, const QString& newName);
    void togglePause(int index);
    void startAll();
    void stopAll();
    void viewLog(int index);

signals:
    // Tín hiệu cho Model
    void queueChanged();
    void itemChanged(int row);

    // Tín hiệu cho Controller
    void logMessageReady(const QString& logContent);

    // Tín hiệu cho các module khác
    void downloadCompleted();
    void downloadProgressUpdated(int videoId, int percentage, const QString& status);


private slots:
    void onTaskProgressUpdated(const QString& uuid, int percentage, qint64 size, const QString& speed, const QString& eta);
    void onTaskFinished(const QString& uuid, bool success, const QString& finalPath);
    void onTaskErrorOccurred(const QString& uuid, const QString& errorMessage);
    void onTaskTitleResolved(const QString& uuid, const QString& title);
    void onTaskRawOutputReady(const QString& uuid, const QString& output);

private:
    void startNextQueuedItem();
    int findItemByUuid(const QString& uuid);
    void updateTaskbarProgress();
    void loadQueue();
    void saveQueue();
    
    DownloadManager* m_downloadManager;
    TaskbarManager* m_taskbarManager = nullptr;
    QList<DownloadItem> m_downloadQueue;
    bool m_isStopped = false; // Thêm biến thành viên bị thiếu
};

