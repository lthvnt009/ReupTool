// Vị trí: /src/interfaces/idatabasemanager.h
// Phiên bản: 1.8 (Thêm chức năng ẩn kênh)

#pragma once

#include <QList>
#include <QString>
#include <QStringList>
#include <QDate>
#include "../models.h" 

class IDatabaseManager
{
public:
    virtual ~IDatabaseManager() = default;

    virtual bool isDbOpen() const = 0;

    virtual QList<Channel> loadChannels() = 0;
    virtual QList<Channel> loadVisibleChannels() = 0; // Hàm mới
    virtual Channel getChannelById(int id) = 0;
    virtual bool addChannel(const QString& name, const QString& link) = 0;
    virtual bool updateChannel(int id, const QString& newName, const QString& newLink) = 0;
    virtual bool updateChannelHiddenState(int channelId, bool isHidden) = 0; // Hàm mới
    virtual bool deleteChannel(int id) = 0;
    virtual bool updateChannelsOrder(const QList<Channel>& channels) = 0;

    virtual bool addVideos(int channelId, const QStringList& urls) = 0;
    virtual bool saveVideoData(const Video& videoData) = 0;
    virtual QList<int> saveVideosAndGetIds(const QList<Video>& videos) = 0;
    virtual QList<Video> loadVideos(int channelId) = 0;
    virtual QList<Video> loadPendingVideos() = 0;
    virtual QList<Video> getVideosForDates(const QList<int>& channelIds, const QDate& startDate, const QDate& endDate) = 0;
    virtual Video getNextPendingVideo() = 0;
    virtual bool videoUrlExists(const QString& url) = 0;
    virtual QList<Video> loadCompletedVideos() = 0;
    virtual bool updateVideoStatus(int videoId, const QString& status) = 0;
    virtual bool markVideoAsUploaded(int videoId) = 0;
    virtual bool updateVideoDownloadedInfo(int videoId, const QString& status, const QString& filePath, const QString& originalTitle) = 0;
    virtual bool updateVideoUploadInfo(int videoId, const QString& newTitle, const QString& newDescription, const QString& newTags, const QString& newSubTags, const QString& newPlaylist, const QString& newCategory) = 0;
    virtual int getUploadedVideoCount(int channelId, const QDate& startDate, const QDate& endDate) = 0;
};
