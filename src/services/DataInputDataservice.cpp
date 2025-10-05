// Vị trí: /src/services/datainputdataservice.cpp
// Phiên bản: 1.2 (Tái cấu trúc CSDL)

#include "datainputdataservice.h"
#include "../repositories/ivideorepository.h"

DataInputDataService::DataInputDataService(IVideoRepository* videoRepo, QObject *parent)
    : QObject(parent), m_videoRepo(videoRepo)
{
}

QList<Video> DataInputDataService::getVideosForDates(const QList<int>& channelIds, const QDate& startDate, const QDate& endDate)
{
    if (!m_videoRepo) return {};
    return m_videoRepo->getVideosForDates(channelIds, startDate, endDate);
}

QList<int> DataInputDataService::saveDataAndGetIds(const QList<Video>& videosToSave)
{
    if (!m_videoRepo) return {};
    return m_videoRepo->saveVideosAndGetIds(videosToSave);
}

void DataInputDataService::addPendingPlaylistUrls(int channelId, const QStringList& urls)
{
    m_pendingPlaylistUrls[channelId] = urls;
}

QStringList DataInputDataService::getPendingPlaylistUrls(int channelId) const
{
    return m_pendingPlaylistUrls.value(channelId);
}

void DataInputDataService::clearPendingPlaylistUrls(int channelId)
{
    m_pendingPlaylistUrls.remove(channelId);
}

QList<int> DataInputDataService::getPendingPlaylistChannelIds() const
{
    return m_pendingPlaylistUrls.keys();
}

bool DataInputDataService::videoUrlExists(const QString& url)
{
    if (!m_videoRepo) return false;
    return m_videoRepo->videoUrlExists(url);
}
