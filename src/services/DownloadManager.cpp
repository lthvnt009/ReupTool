// Phiên bản: 1.3 (Sửa lỗi build)
// Vị trí: /src/services/downloadmanager.cpp

#include "downloadmanager.h"
#include "downloadtask.h"
#include <QDebug>

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
    , m_threadPool(new QThreadPool(this))
{
    m_threadPool->setMaxThreadCount(1); // Mặc định là 1
}

DownloadManager::~DownloadManager()
{
    // Đảm bảo tất cả các luồng đã dừng trước khi thoát
    m_threadPool->clear();
    m_threadPool->waitForDone();
}

void DownloadManager::startDownload(const DownloadItem& item)
{
    DownloadTask* task = new DownloadTask(item);

    // Kết nối các tín hiệu từ task đến manager để chuyển tiếp
    connect(task, &DownloadTask::progressUpdated, this, &DownloadManager::taskProgressUpdated);
    connect(task, &DownloadTask::finished, this, &DownloadManager::taskFinished);
    connect(task, &DownloadTask::errorOccurred, this, &DownloadManager::taskErrorOccurred);
    connect(task, &DownloadTask::titleResolved, this, &DownloadManager::taskTitleResolved);
    connect(task, &DownloadTask::rawOutputReady, this, &DownloadManager::taskRawOutputReady);

    // Kết nối tín hiệu hoàn thành của task với một slot private để dọn dẹp
    connect(task, &DownloadTask::finished, this, &DownloadManager::onTaskFinished);

    {
        QMutexLocker locker(&m_mutex);
        m_activeTasks.append(task);
    }

    m_threadPool->start(task);
}

void DownloadManager::stopDownload(const QString& uuid)
{
    QMutexLocker locker(&m_mutex);
    for (DownloadTask* task : m_activeTasks) {
        if (task->uuid() == uuid) {
            task->stop();
            break;
        }
    }
}

void DownloadManager::setMaxConcurrentDownloads(int count)
{
    m_threadPool->setMaxThreadCount(count);
}

bool DownloadManager::canStartNewTask() const
{
    return m_activeTasks.size() < m_threadPool->maxThreadCount();
}

// *** SỬA LỖI BUILD: Thêm định nghĩa hàm ***
int DownloadManager::activeTaskCount() const
{
    QMutexLocker locker(&m_mutex);
    return m_activeTasks.size();
}

void DownloadManager::onTaskFinished(const QString& uuid)
{
    QMutexLocker locker(&m_mutex);
    for (int i = 0; i < m_activeTasks.size(); ++i) {
        if (m_activeTasks[i]->uuid() == uuid) {
            m_activeTasks.removeAt(i);
            // Task sẽ tự xóa khi run() kết thúc
            break;
        }
    }
}

