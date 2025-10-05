// Vị trí: /src/databasemanager.h
// Phiên bản: 3.3 (Sửa lỗi Build)

#pragma once

#include <QSqlDatabase>
#include <memory>

// Forward declarations
class IChannelRepository;
class IVideoRepository;
class ChannelRepository;
class VideoRepository;

class DatabaseManager
{
public:
    explicit DatabaseManager(const QString& fullPath);
    ~DatabaseManager();

    bool isDbOpen() const;

    // Getters for repositories
    IChannelRepository* getChannelRepository() const;
    IVideoRepository* getVideoRepository() const;

private:
    bool openDatabase();
    bool createTables();
    void runMigrations();
    bool createChannelsTable();
    bool createVideosTable();
    bool createVideosFtsTable(); // Thêm khai báo

    QSqlDatabase m_db;
    std::unique_ptr<ChannelRepository> m_channelRepo;
    std::unique_ptr<VideoRepository> m_videoRepo;
};

