// Vị trí: /src/services/datainputdataservice.h
// Phiên bản: 1.2 (Tái cấu trúc CSDL)

#pragma once

#include <QObject>
#include <QMap>
#include <QStringList>
#include "../models.h"

class IVideoRepository;

class DataInputDataService : public QObject
{
    Q_OBJECT
public:
    explicit DataInputDataService(IVideoRepository* videoRepo, QObject *parent = nullptr);

    QList<Video> getVideosForDates(const QList<int>& channelIds, const QDate& startDate, const QDate& endDate);
    QList<int> saveDataAndGetIds(const QList<Video>& videosToSave);
    void addPendingPlaylistUrls(int channelId, const QStringList& urls);
    QStringList getPendingPlaylistUrls(int channelId) const;
    void clearPendingPlaylistUrls(int channelId);
    QList<int> getPendingPlaylistChannelIds() const;
    bool videoUrlExists(const QString& url);

private:
    IVideoRepository* m_videoRepo;
    QMap<int, QStringList> m_pendingPlaylistUrls;
};
