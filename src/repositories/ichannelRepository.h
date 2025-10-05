// Vị trí: /src/repositories/ichannelrepository.h
// Phiên bản: 1.2 (Thêm chức năng kiểm tra link tồn tại)
// Mô tả: Interface định nghĩa các thao tác CSDL cho Kênh.

#pragma once

#include <QList>
#include "../models.h"

class IChannelRepository
{
public:
    virtual ~IChannelRepository() = default;

    virtual QList<Channel> loadAllChannels() = 0;
    virtual QList<Channel> loadVisibleChannels() = 0;
    virtual Channel getChannelById(int id) = 0;
    virtual bool addChannel(const QString& name, const QString& link) = 0;
    virtual bool updateChannel(const Channel& channel) = 0;
    virtual bool deleteChannel(int id) = 0;
    virtual bool updateChannelsOrder(const QList<Channel>& channels) = 0;
    virtual bool setChannelHidden(int channelId, bool isHidden) = 0;
    virtual bool channelLinkExists(const QString& link) const = 0;
};

