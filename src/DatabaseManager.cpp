// Vị trí: /src/databasemanager.cpp
// Phiên bản: 3.4 (Thêm Undo & Sửa lỗi Build)

#include "databasemanager.h"
#include "repositories/channelrepository.h"
#include "repositories/videorepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlRecord>
#include <QDir>

DatabaseManager::DatabaseManager(const QString& fullPath)
{
    const QString connectionName = "ReupToolDBConnection";
    if (QSqlDatabase::contains(connectionName)) {
        m_db = QSqlDatabase::database(connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    }

    m_db.setDatabaseName(fullPath);

    if (openDatabase()) {
        m_db.exec("PRAGMA foreign_keys = ON;");
        createTables();
        runMigrations();
    }

    // Khởi tạo repositories sau khi đã có kết nối CSDL
    m_channelRepo = std::make_unique<ChannelRepository>(m_db);
    m_videoRepo = std::make_unique<VideoRepository>(m_db);
}

DatabaseManager::~DatabaseManager()
{
    // unique_ptr sẽ tự động dọn dẹp
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::isDbOpen() const
{
    return m_db.isOpen();
}

IChannelRepository* DatabaseManager::getChannelRepository() const
{
    return m_channelRepo.get();
}

IVideoRepository* DatabaseManager::getVideoRepository() const
{
    return m_videoRepo.get();
}

bool DatabaseManager::openDatabase()
{
    if (!m_db.open()) {
        qCritical() << "Database connection error:" << m_db.lastError().text();
        return false;
    }
    qInfo() << "Database opened successfully at:" << m_db.databaseName();
    return true;
}

bool DatabaseManager::createTables()
{
    bool success = createChannelsTable();
    success &= createVideosTable();
    success &= createVideosFtsTable();
    return success;
}

void DatabaseManager::runMigrations()
{
    // Migration cho bảng channels
    QSqlRecord channelRecord = m_db.record("channels");
    if (channelRecord.indexOf("display_order") == -1) {
        qInfo() << "Applying migration: Adding 'display_order' to 'channels' table...";
        QSqlQuery alterQuery(m_db);
        if (!alterQuery.exec("ALTER TABLE channels ADD COLUMN display_order INTEGER NOT NULL DEFAULT 0")) {
            qCritical() << "Failed to add 'display_order' column to channels table:" << alterQuery.lastError().text();
        }
    }
    if (channelRecord.indexOf("is_hidden") == -1) {
        qInfo() << "Applying migration: Adding 'is_hidden' to 'channels' table...";
        QSqlQuery alterQuery(m_db);
        if (!alterQuery.exec("ALTER TABLE channels ADD COLUMN is_hidden INTEGER NOT NULL DEFAULT 0")) {
            qCritical() << "Failed to add 'is_hidden' column to channels table:" << alterQuery.lastError().text();
        }
    }
    
    // Migration cho bảng videos
    QSqlRecord videoRecord = m_db.record("videos");
    if (videoRecord.indexOf("upload_date") == -1) {
        qInfo() << "Applying migration: Adding 'upload_date' to 'videos' table...";
        QSqlQuery alterQuery(m_db);
        if (!alterQuery.exec("ALTER TABLE videos ADD COLUMN upload_date TEXT")) {
            qCritical() << "Failed to add 'upload_date' column to videos table:" << alterQuery.lastError().text();
        }
    }
    if (videoRecord.indexOf("new_category") == -1) {
        qInfo() << "Applying migration: Adding 'new_category' to 'videos' table...";
        QSqlQuery alterQuery(m_db);
        if (!alterQuery.exec("ALTER TABLE videos ADD COLUMN new_category TEXT")) {
            qCritical() << "Failed to add 'new_category' column to videos table:" << alterQuery.lastError().text();
        }
    }
    if (videoRecord.indexOf("video_date") == -1) {
        qInfo() << "Applying migration: Adding 'video_date' to 'videos' table...";
        QSqlQuery alterQuery(m_db);
        if (!alterQuery.exec("ALTER TABLE videos ADD COLUMN video_date TEXT")) {
            qCritical() << "Failed to add 'video_date' column to videos table:" << alterQuery.lastError().text();
        }
    }
    // Migration cho tính năng Undo
    if (videoRecord.indexOf("is_deleted") == -1) {
        qInfo() << "Applying migration: Adding 'is_deleted' and 'deletion_date' to 'videos' table...";
        QSqlQuery alterQuery(m_db);
        if (!alterQuery.exec("ALTER TABLE videos ADD COLUMN is_deleted INTEGER NOT NULL DEFAULT 0")) {
            qCritical() << "Failed to add 'is_deleted' column to videos table:" << alterQuery.lastError().text();
        }
        if (!alterQuery.exec("ALTER TABLE videos ADD COLUMN deletion_date TEXT")) {
            qCritical() << "Failed to add 'deletion_date' column to videos table:" << alterQuery.lastError().text();
        }
    }
}

bool DatabaseManager::createChannelsTable()
{
    QSqlQuery query(m_db);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS channels ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "name TEXT NOT NULL, "
                              "link TEXT NOT NULL, "
                              "display_order INTEGER NOT NULL DEFAULT 0, "
                              "is_hidden INTEGER NOT NULL DEFAULT 0"
                              ");");
    if (!success) {
        qCritical() << "Failed to create 'channels' table:" << query.lastError().text();
    }
    return success;
}

bool DatabaseManager::createVideosTable()
{
    QSqlQuery query(m_db);
    bool success = query.exec("CREATE TABLE IF NOT EXISTS videos ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                              "channel_id INTEGER NOT NULL, "
                              "video_url TEXT NOT NULL UNIQUE, "
                              "video_date TEXT, "
                              "original_title TEXT, "
                              "status TEXT NOT NULL DEFAULT 'Pending', "
                              "added_date TEXT NOT NULL, "
                              "local_file_path TEXT, "
                              "new_title TEXT, "
                              "new_description TEXT, "
                              "new_tags TEXT, "
                              "new_sub_tags TEXT, "
                              "new_playlist TEXT, "
                              "upload_date TEXT, "
                              "new_category TEXT, "
                              "is_deleted INTEGER NOT NULL DEFAULT 0, "
                              "deletion_date TEXT, "
                              "FOREIGN KEY(channel_id) REFERENCES channels(id) ON DELETE CASCADE"
                              ");");
    if (!success) {
        qCritical() << "Failed to create 'videos' table:" << query.lastError().text();
    }
    return success;
}

bool DatabaseManager::createVideosFtsTable()
{
    QSqlQuery query(m_db);
    // Tạo bảng FTS nếu chưa tồn tại
    bool success = query.exec("CREATE VIRTUAL TABLE IF NOT EXISTS videos_fts USING fts5("
                              "new_title, "
                              "new_description, "
                              "new_tags, "
                              "content='videos', "
                              "content_rowid='id'"
                              ");");
    if (!success) {
        qCritical() << "Failed to create 'videos_fts' virtual table:" << query.lastError().text();
        return false;
    }

    // Tạo triggers để tự động đồng bộ
    const QStringList triggers = {
        "CREATE TRIGGER IF NOT EXISTS videos_after_insert AFTER INSERT ON videos BEGIN "
        "  INSERT INTO videos_fts(rowid, new_title, new_description, new_tags) VALUES (new.id, new.new_title, new.new_description, new.new_tags); "
        "END;",
        "CREATE TRIGGER IF NOT EXISTS videos_after_delete AFTER DELETE ON videos BEGIN "
        "  INSERT INTO videos_fts(videos_fts, rowid, new_title, new_description, new_tags) VALUES ('delete', old.id, old.new_title, old.new_description, old.new_tags); "
        "END;",
        "CREATE TRIGGER IF NOT EXISTS videos_after_update AFTER UPDATE ON videos BEGIN "
        "  INSERT INTO videos_fts(videos_fts, rowid, new_title, new_description, new_tags) VALUES ('delete', old.id, old.new_title, old.new_description, old.new_tags); "
        "  INSERT INTO videos_fts(rowid, new_title, new_description, new_tags) VALUES (new.id, new.new_title, new.new_description, new.new_tags); "
        "END;"
    };

    for(const QString& triggerSql : triggers) {
        if (!query.exec(triggerSql)) {
            qCritical() << "Failed to create FTS trigger:" << query.lastError().text();
            // Không trả về false ngay, có thể trigger đã tồn tại từ lần chạy trước
        }
    }

    return true;
}

