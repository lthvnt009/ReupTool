// Phiên bản: 1.3 (Sử dụng AppSettings)
// Vị trí: /src/services/downloadqueueservice.cpp

#include "downloadqueueservice.h"
#include "downloadmanager.h"
#include "taskbarmanager.h"
#include "../appsettings.h"
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QFileInfo>
#include <QSettings>
#include <QDebug>

DownloadQueueService::DownloadQueueService(QObject *parent)
    : QObject(parent)
    , m_downloadManager(new DownloadManager(this))
{
    // Tìm cửa sổ chính để gắn TaskbarManager
    QWidget* topLevelWidget = nullptr;
    foreach(QWidget *widget, QApplication::topLevelWidgets()) {
        if (qobject_cast<QMainWindow*>(widget)) {
            topLevelWidget = widget;
            break;
        }
    }

#ifdef Q_OS_WIN
    if(topLevelWidget) {
        m_taskbarManager = new TaskbarManager(topLevelWidget);
    }
#endif

    connect(m_downloadManager, &DownloadManager::taskProgressUpdated, this, &DownloadQueueService::onTaskProgressUpdated);
    connect(m_downloadManager, &DownloadManager::taskFinished, this, &DownloadQueueService::onTaskFinished);
    connect(m_downloadManager, &DownloadManager::taskErrorOccurred, this, &DownloadQueueService::onTaskErrorOccurred);
    connect(m_downloadManager, &DownloadManager::taskTitleResolved, this, &DownloadQueueService::onTaskTitleResolved);
    connect(m_downloadManager, &DownloadManager::taskRawOutputReady, this, &DownloadQueueService::onTaskRawOutputReady);

    loadQueue();
}

DownloadQueueService::~DownloadQueueService()
{
    saveQueue();
}

QList<DownloadItem>* DownloadQueueService::getQueue()
{
    return &m_downloadQueue;
}

const DownloadItem& DownloadQueueService::getItem(int index) const
{
    static const DownloadItem invalidItem;
    if (index < 0 || index >= m_downloadQueue.size()) {
        return invalidItem;
    }
    return m_downloadQueue.at(index);
}

void DownloadQueueService::setMaxConcurrentDownloads(int count)
{
    m_downloadManager->setMaxConcurrentDownloads(count);
}

void DownloadQueueService::addItem(const QString& url, const DownloadOptions& options)
{
    DownloadItem newItem;
    newItem.url = url;
    newItem.options = options;
    m_downloadQueue.append(newItem);
    emit queueChanged();
}

void DownloadQueueService::addLegacyDownloads(const QList<Video>& videos, const DownloadOptions& options)
{
    for(const auto& video : videos) {
        bool alreadyInQueue = false;
        for(const auto& item : m_downloadQueue) {
            if(item.dbVideoId == video.id && video.id != -1) { // Đảm bảo ID hợp lệ
                alreadyInQueue = true;
                break;
            }
        }
        if(alreadyInQueue) continue;

        DownloadItem newItem;
        newItem.dbVideoId = video.id;
        newItem.url = video.videoUrl;
        newItem.title = video.originalTitle.isEmpty() ? video.videoUrl : video.originalTitle;
        newItem.status = DownloadStatus::Queued;
        newItem.options = options;

        m_downloadQueue.append(newItem);
    }
    emit queueChanged();
    startAll();
}


void DownloadQueueService::removeItem(int index)
{
    if (index < 0 || index >= m_downloadQueue.size()) return;
    
    m_downloadManager->stopDownload(m_downloadQueue[index].uuid);
    m_downloadQueue.removeAt(index);
    emit queueChanged();
    startNextQueuedItem(); // Bắt đầu mục tiếp theo nếu có chỗ trống
}

void DownloadQueueService::skipItem(int index)
{
    if (index < 0 || index >= m_downloadQueue.size()) return;

    m_downloadManager->stopDownload(m_downloadQueue[index].uuid);
    m_downloadQueue[index].status = DownloadStatus::Skipped;
    emit itemChanged(index);
    startNextQueuedItem();
}

void DownloadQueueService::refreshItem(int index)
{
    if (index < 0 || index >= m_downloadQueue.size()) return;
    
    if (m_downloadQueue[index].status == DownloadStatus::Error) {
        m_downloadQueue[index].status = DownloadStatus::Queued;
        m_downloadQueue[index].errorMessage.clear();
        m_downloadQueue[index].progress = 0;
        m_downloadQueue[index].logOutput.clear();
        emit itemChanged(index);
        startNextQueuedItem();
    }
}

void DownloadQueueService::renameItem(int index, const QString& newName)
{
    if (index < 0 || index >= m_downloadQueue.size()) return;

    DownloadItem& item = m_downloadQueue[index];
    QString ext = QFileInfo(item.options.outputTemplate).suffix();
    if (ext.isEmpty()) {
        ext = item.options.format.toLower();
    }
    item.options.outputTemplate = newName + "." + ext;
    item.title = newName;
    emit itemChanged(index);
}


void DownloadQueueService::togglePause(int index)
{
    if (index < 0 || index >= m_downloadQueue.size()) return;

    if (m_downloadQueue[index].status == DownloadStatus::Downloading) {
        m_downloadQueue[index].status = DownloadStatus::Paused;
        m_downloadManager->stopDownload(m_downloadQueue[index].uuid);
        emit itemChanged(index);
        startNextQueuedItem();
    } else {
        m_downloadQueue[index].status = DownloadStatus::Queued;
        emit itemChanged(index);
        startNextQueuedItem();
    }
}


void DownloadQueueService::startAll()
{
    m_isStopped = false; // Reset cờ dừng
    for (int i = 0; i < m_downloadQueue.size(); ++i) {
        if (m_downloadQueue[i].status == DownloadStatus::Paused || m_downloadQueue[i].status == DownloadStatus::Stopped || m_downloadQueue[i].status == DownloadStatus::Error) {
             m_downloadQueue[i].status = DownloadStatus::Queued;
             emit itemChanged(i);
        }
    }
    startNextQueuedItem();
}

void DownloadQueueService::stopAll()
{
    m_isStopped = true; // Set cờ dừng
    for (int i = 0; i < m_downloadQueue.size(); ++i) {
        if (m_downloadQueue[i].status == DownloadStatus::Downloading) {
             m_downloadManager->stopDownload(m_downloadQueue[i].uuid);
             // Trạng thái sẽ được cập nhật trong onTaskFinished/onTaskError
        }
         else if (m_downloadQueue[i].status == DownloadStatus::Queued) {
            m_downloadQueue[i].status = DownloadStatus::Stopped;
            emit itemChanged(i);
        }
    }
}

void DownloadQueueService::viewLog(int index)
{
     if (index < 0 || index >= m_downloadQueue.size()) return;
     emit logMessageReady(m_downloadQueue[index].logOutput);
}

void DownloadQueueService::onTaskProgressUpdated(const QString& uuid, int percentage, qint64 size, const QString& speed, const QString& eta)
{
    int row = findItemByUuid(uuid);
    if (row != -1) {
        m_downloadQueue[row].progress = percentage;
        m_downloadQueue[row].size = size;
        m_downloadQueue[row].speed = speed;
        m_downloadQueue[row].timeRemaining = eta;
        emit itemChanged(row);

        if (m_downloadQueue[row].dbVideoId != -1) {
            emit downloadProgressUpdated(m_downloadQueue[row].dbVideoId, percentage, statusToString(m_downloadQueue[row].status));
        }
    }
    updateTaskbarProgress();
}

void DownloadQueueService::onTaskFinished(const QString& uuid, bool success, const QString& finalPath)
{
    int row = findItemByUuid(uuid);
    if (row != -1) {
        if(m_downloadQueue[row].status == DownloadStatus::Paused) {
             // Bị người dùng tạm dừng, không làm gì
        }
        else if(m_isStopped) { // Bị người dùng dừng hẳn
             m_downloadQueue[row].status = DownloadStatus::Stopped;
        } else {
            m_downloadQueue[row].status = success ? DownloadStatus::Completed : DownloadStatus::Error;
            if (success) {
                m_downloadQueue[row].localFilePath = finalPath;
            }
            else if (m_downloadQueue[row].errorMessage.isEmpty()){
                 m_downloadQueue[row].errorMessage = "Tải về không thành công.";
            }
        }
        
        emit itemChanged(row);

        if (m_downloadQueue[row].dbVideoId != -1 && success) {
            emit downloadCompleted();
            emit downloadProgressUpdated(m_downloadQueue[row].dbVideoId, 100, "Completed");
        }

        startNextQueuedItem();
    }
    updateTaskbarProgress();
}

void DownloadQueueService::onTaskErrorOccurred(const QString& uuid, const QString& errorMessage)
{
    int row = findItemByUuid(uuid);
    if (row != -1) {
        if (m_downloadQueue[row].status == DownloadStatus::Paused || m_isStopped) return;

        m_downloadQueue[row].status = DownloadStatus::Error;
        m_downloadQueue[row].errorMessage = errorMessage;
        emit itemChanged(row);

        if (m_downloadQueue[row].dbVideoId != -1) {
             emit downloadProgressUpdated(m_downloadQueue[row].dbVideoId, 0, "Lỗi");
        }
        
        startNextQueuedItem();
    }
    updateTaskbarProgress();
}

void DownloadQueueService::onTaskTitleResolved(const QString& uuid, const QString& title)
{
    int row = findItemByUuid(uuid);
    if (row != -1) {
        m_downloadQueue[row].title = title;
        emit itemChanged(row);
    }
}

void DownloadQueueService::onTaskRawOutputReady(const QString& uuid, const QString& output)
{
    int row = findItemByUuid(uuid);
    if (row != -1) {
        m_downloadQueue[row].logOutput.append(output);
        // Không phát tín hiệu ở đây để tránh làm chậm UI, chỉ phát khi người dùng yêu cầu xem log
    }
}

void DownloadQueueService::startNextQueuedItem()
{
    if (m_isStopped) return; // Không bắt đầu item mới nếu đã nhấn Dừng tất cả
    for (int i = 0; i < m_downloadQueue.size(); ++i) {
        if (m_downloadQueue[i].status == DownloadStatus::Queued) {
            if(m_downloadManager->canStartNewTask()) {
                m_downloadQueue[i].status = DownloadStatus::Downloading;
                emit itemChanged(i);
                m_downloadManager->startDownload(m_downloadQueue[i]);
            }
        }
    }
    updateTaskbarProgress();
}

int DownloadQueueService::findItemByUuid(const QString& uuid)
{
    for (int i = 0; i < m_downloadQueue.size(); ++i) {
        if (m_downloadQueue[i].uuid == uuid) {
            return i;
        }
    }
    return -1;
}

void DownloadQueueService::updateTaskbarProgress()
{
#ifdef Q_OS_WIN
    if (!m_taskbarManager) return;

    int totalProgress = 0;
    int downloadingCount = 0;
    bool hasError = false;

    for(const auto& item : m_downloadQueue) {
        if(item.status == DownloadStatus::Downloading) {
            totalProgress += item.progress;
            downloadingCount++;
        }
        if(item.status == DownloadStatus::Error) {
            hasError = true;
        }
    }
    
    if (downloadingCount > 0) {
        if(hasError) {
             m_taskbarManager->setProgressState(TBPF_PAUSED);
        } else {
            m_taskbarManager->setProgressState(TBPF_NORMAL);
        }
        m_taskbarManager->setProgressValue(totalProgress, downloadingCount * 100);
    } else {
        if (hasError) {
            m_taskbarManager->setProgressState(TBPF_ERROR);
        } else {
            m_taskbarManager->setProgressState(TBPF_NOPROGRESS);
        }
    }
#endif
}


void DownloadQueueService::loadQueue()
{
    QSettings& settings = getAppSettings();
    int size = settings.beginReadArray("DownloadQueue");
    m_downloadQueue.clear();
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        DownloadItem item;
        item.uuid = settings.value("uuid").toString();
        item.url = settings.value("url").toString();
        item.title = settings.value("title").toString();
        // Giữ lại trạng thái Paused hoặc Stopped
        QString statusStr = settings.value("statusStr", "Queued").toString();
        if(statusStr == "Paused") item.status = DownloadStatus::Paused;
        else if (statusStr == "Stopped") item.status = DownloadStatus::Stopped;
        else item.status = DownloadStatus::Queued;

        item.dbVideoId = settings.value("dbVideoId", -1).toInt();
        item.options.savePath = settings.value("savePath").toString();
        item.options.outputTemplate = settings.value("outputTemplate").toString();
        
        m_downloadQueue.append(item);
    }
    settings.endArray();
    emit queueChanged();
}

void DownloadQueueService::saveQueue()
{
    QSettings& settings = getAppSettings();
    settings.remove("DownloadQueue");
    settings.beginWriteArray("DownloadQueue");
    int index = 0;
    for (const auto& item : m_downloadQueue) {
        // Chỉ lưu các mục chưa hoàn thành
        if (item.status != DownloadStatus::Completed && item.status != DownloadStatus::Skipped) {
            settings.setArrayIndex(index++);
            settings.setValue("uuid", item.uuid);
            settings.setValue("url", item.url);
            settings.setValue("title", item.title);
            settings.setValue("dbVideoId", item.dbVideoId);
            settings.setValue("savePath", item.options.savePath);
            settings.setValue("outputTemplate", item.options.outputTemplate);
            if(item.status == DownloadStatus::Paused) settings.setValue("statusStr", "Paused");
            else if (item.status == DownloadStatus::Stopped) settings.setValue("statusStr", "Stopped");

        }
    }
    settings.endArray();
}
