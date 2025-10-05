// Vị trí: /src/repositories/channelrepository.h
// Phiên bản: 1.2 (Thêm chức năng kiểm tra link tồn tại)
// Mô tả: Lớp triển khai các thao tác CSDL cho Kênh.

#pragma once

#include "ichannelrepository.h"
#include <QSqlDatabase>

class ChannelRepository : public IChannelRepository
{
public:
    explicit ChannelRepository(QSqlDatabase& db);

    QList<Channel> loadAllChannels() override;
    QList<Channel> loadVisibleChannels() override;
    Channel getChannelById(int id) override;
    bool addChannel(const QString& name, const QString& link) override;
    bool updateChannel(const Channel& channel) override;
    bool deleteChannel(int id) override;
    bool updateChannelsOrder(const QList<Channel>& channels) override;
    bool setChannelHidden(int channelId, bool isHidden) override;
    bool channelLinkExists(const QString& link) const override;

private:
    QSqlDatabase& m_db;
};

