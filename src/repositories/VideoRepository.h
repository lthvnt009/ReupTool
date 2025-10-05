// Vị trí: /src/repositories/videorepository.h
// Phiên bản: 1.7 (Sửa lỗi Build)

#pragma once

#include "ivideorepository.h"
#include <QSqlDatabase>

class VideoRepository : public IVideoRepository
{
public:
    explicit VideoRepository(QSqlDatabase& db);

    Video getVideoById(int id) override;
    QList<Video> getVideos(const FilterCriteria& criteria) override;
    QList<Video> getAllVideos(const FilterCriteria& criteria) override;
    int getVideoCount(const FilterCriteria& criteria) override;
    QList<int> saveOrUpdateVideos(const QList<Video>& videos) override;
    bool updateVideo(const Video& video) override;
    bool videoUrlExists(const QString& url) override;
    QList<Video> loadPendingVideos() override;
    QList<Video> loadCompletedVideos() override;
    bool markVideoAsUploaded(int videoId) override;
    bool updateVideoDownloadedInfo(int videoId, const QString& status, const QString& filePath, const QString& originalTitle) override;
    bool updateChannelForVideos(const QList<int>& videoIds, int newChannelId) override;
    int getUploadedVideoCount(int channelId, const QDate& startDate, const QDate& endDate) override;
    QList<int> saveVideosAndGetIds(const QList<Video>& videos) override;

    // Methods needed by older modules
    QList<Video> getVideosForDates(const QList<int>& channelIds, const QDate& startDate, const QDate& endDate) override;
    bool updateVideoUploadInfo(const Video& video) override;

    // Soft delete methods
    bool deleteVideos(const QList<int>& videoIds, bool permanently) override;
    bool restoreVideos(const QList<int>& videoIds) override;
    int getSoftDeletedVideoCount() override;
    int clearDeletedVideos() override;

    // Maintenance methods
    QMap<QString, QList<Video>> findDuplicateVideos() override;


private:
    Video parseVideoFromQuery(class QSqlQuery& query);
    QSqlDatabase& m_db;
};

