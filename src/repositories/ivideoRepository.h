// Vị trí: /src/repositories/ivideorepository.h
// Phiên bản: 1.7 (Sửa lỗi Build)

#pragma once

#include <QList>
#include <QDate>
#include "../models.h"

class IVideoRepository
{
public:
    virtual ~IVideoRepository() = default;

    virtual Video getVideoById(int id) = 0;
    virtual QList<Video> getVideos(const FilterCriteria& criteria) = 0;
    virtual QList<Video> getAllVideos(const FilterCriteria& criteria) = 0;
    virtual int getVideoCount(const FilterCriteria& criteria) = 0;
    virtual QList<int> saveOrUpdateVideos(const QList<Video>& videos) = 0;
    virtual bool updateVideo(const Video& video) = 0;
    virtual bool videoUrlExists(const QString& url) = 0;
    virtual QList<Video> loadPendingVideos() = 0;
    virtual QList<Video> loadCompletedVideos() = 0;
    virtual bool markVideoAsUploaded(int videoId) = 0;
    virtual bool updateVideoDownloadedInfo(int videoId, const QString& status, const QString& filePath, const QString& originalTitle) = 0;
    virtual bool updateChannelForVideos(const QList<int>& videoIds, int newChannelId) = 0;
    virtual int getUploadedVideoCount(int channelId, const QDate& startDate, const QDate& endDate) = 0;
    virtual QList<int> saveVideosAndGetIds(const QList<Video>& videos) = 0;

    // Methods needed by older modules
    virtual QList<Video> getVideosForDates(const QList<int>& channelIds, const QDate& startDate, const QDate& endDate) = 0;
    virtual bool updateVideoUploadInfo(const Video& video) = 0;

    // Soft delete methods
    virtual bool deleteVideos(const QList<int>& videoIds, bool permanently) = 0;
    virtual bool restoreVideos(const QList<int>& videoIds) = 0;
    virtual int getSoftDeletedVideoCount() = 0;
    virtual int clearDeletedVideos() = 0;

    // Maintenance methods
    virtual QMap<QString, QList<Video>> findDuplicateVideos() = 0;
};

