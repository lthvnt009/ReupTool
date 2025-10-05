// Vị trí: /src/services/LibraryService.h
// Phiên bản: 1.8 (Tối ưu hóa Signal)
// Mô tả: Lớp chứa logic nghiệp vụ cho tab Thư viện.

#pragma once

#include <QObject>
#include "../models.h"
#include <QList>
#include <QMap>

// Forward declarations
class IVideoRepository;
class IChannelRepository;

class LibraryService : public QObject
{
    Q_OBJECT
public:
    explicit LibraryService(IVideoRepository* videoRepo, IChannelRepository* channelRepo, QObject *parent = nullptr);

public slots:
    void requestData(const FilterCriteria& criteria);
    void requestImport(const QString& filePath);
    void requestExport(const QString& filePath, const FilterCriteria& criteria);
    void requestBatchEditTags(const QList<int>& videoIds, int action, const QString& tag1, const QString& tag2);
    void requestBatchChangeChannel(const QList<int>& videoIds, int newChannelId);
    void requestUpdateVideo(const Video& video);
    void requestDelete(const QList<int>& videoIds, bool deleteFiles);
    void requestRestoreLastDeleted();
    void requestCleanupInfo();
    void requestCleanup();
    void requestFindDuplicates();


signals:
    void videosReady(const QList<Video>& videos);
    void countReady(int totalItems, int itemsOnPage);
    void deleteCompleted(bool success, const QString& message, const QList<int>& deletedIds);
    void restoreCompleted(bool success, const QString& message);
    void batchEditCompleted(bool success, const QString& message);
    void taskProgress(int value);
    void taskFinished(bool success, const QString& title, const QString& message);
    void cleanupInfoReady(int count);
    void cleanupCompleted(bool success, const QString& message);
    void duplicatesFound(const QMap<QString, QList<Video>>& duplicates);
    
    // Tín hiệu mới, chi tiết hơn để đồng bộ hóa
    void videosModified(const QList<int>& videoIds);


private:
    IVideoRepository* m_videoRepo;
    IChannelRepository* m_channelRepo;
    QList<int> m_lastDeletedVideoIds;
};

