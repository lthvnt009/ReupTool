// Phiên bản: 1.3 (Sửa lỗi build)
// Vị trí: /src/services/downloadmanager.h
// Mô tả: Lớp quản lý các luồng tải và các tác vụ DownloadTask.

#pragma once

#include <QObject>
#include <QThreadPool>
#include <QList>
#include <QMutex>
#include "../models.h"

// Forward declaration
class DownloadTask;

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    explicit DownloadManager(QObject *parent = nullptr);
    ~DownloadManager();

    void startDownload(const DownloadItem& item);
    void stopDownload(const QString& uuid);
    void setMaxConcurrentDownloads(int count);
    bool canStartNewTask() const;
    int activeTaskCount() const;

signals:
    void taskProgressUpdated(const QString& uuid, int percentage, qint64 size, const QString& speed, const QString& eta);
    void taskFinished(const QString& uuid, bool success, const QString& finalPath);
    void taskErrorOccurred(const QString& uuid, const QString& errorMessage);
    void taskTitleResolved(const QString& uuid, const QString& title);
    void taskRawOutputReady(const QString& uuid, const QString& output);

private slots:
    void onTaskFinished(const QString& uuid);

private:
    QThreadPool* m_threadPool;
    QList<DownloadTask*> m_activeTasks;
    mutable QMutex m_mutex; // *** SỬA LỖI BUILD: Thêm 'mutable' ***
};

