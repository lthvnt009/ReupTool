// Vị trí: /src/repositories/videorepository.cpp
// Phiên bản: 1.7 (Sửa lỗi Build)

#include "videorepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDateTime>
#include <QVariant>
#include <QSqlRecord>

VideoRepository::VideoRepository(QSqlDatabase& db)
    : m_db(db)
{
}

Video VideoRepository::getVideoById(int id)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM videos WHERE id = ? AND is_deleted = 0");
    query.addBindValue(id);

    if (query.exec() && query.next()) {
        return parseVideoFromQuery(query);
    }
    return Video();
}

QList<Video> VideoRepository::getVideos(const FilterCriteria& criteria)
{
    QList<Video> videos;
    QString queryString = "SELECT * FROM videos v ";
    QString whereClause;
    QMap<QString, QVariant> bindValues;

    if (!criteria.searchTerm.isEmpty()) {
        queryString += "JOIN videos_fts fts ON v.id = fts.rowid ";
        whereClause += "videos_fts MATCH :searchTerm ";
        bindValues[":searchTerm"] = criteria.searchTerm + "*";
    }

    if (criteria.channelId > 0) {
        if (!whereClause.isEmpty()) whereClause += "AND ";
        whereClause += "v.channel_id = :channelId ";
        bindValues[":channelId"] = criteria.channelId;
    }

    if (!criteria.status.isEmpty() && criteria.status != "Tất cả") {
        if (!whereClause.isEmpty()) whereClause += "AND ";
        whereClause += "v.status = :status ";
        bindValues[":status"] = criteria.status;
    }

    if (!whereClause.isEmpty()) {
        queryString += "WHERE " + whereClause + "AND v.is_deleted = 0 ";
    } else {
        queryString += "WHERE v.is_deleted = 0 ";
    }

    queryString += "ORDER BY v.id DESC LIMIT :limit OFFSET :offset";
    bindValues[":limit"] = criteria.pageSize;
    bindValues[":offset"] = (criteria.page - 1) * criteria.pageSize;

    QSqlQuery query(m_db);
    query.prepare(queryString);
    for(auto it = bindValues.constBegin(); it != bindValues.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qCritical() << "Failed to get videos:" << query.lastError().text();
        return videos;
    }
    
    while(query.next()) {
        videos.append(parseVideoFromQuery(query));
    }
    return videos;
}

QList<Video> VideoRepository::getAllVideos(const FilterCriteria& criteria)
{
    QList<Video> videos;
    QString queryString = "SELECT * FROM videos v ";
    QString whereClause;
    QMap<QString, QVariant> bindValues;

    if (!criteria.searchTerm.isEmpty()) {
        queryString += "JOIN videos_fts fts ON v.id = fts.rowid ";
        whereClause += "videos_fts MATCH :searchTerm ";
        bindValues[":searchTerm"] = criteria.searchTerm + "*";
    }

    if (criteria.channelId > 0) {
        if (!whereClause.isEmpty()) whereClause += "AND ";
        whereClause += "v.channel_id = :channelId ";
        bindValues[":channelId"] = criteria.channelId;
    }

    if (!criteria.status.isEmpty() && criteria.status != "Tất cả") {
        if (!whereClause.isEmpty()) whereClause += "AND ";
        whereClause += "v.status = :status ";
        bindValues[":status"] = criteria.status;
    }

    if (!whereClause.isEmpty()) {
        queryString += "WHERE " + whereClause + "AND v.is_deleted = 0 ";
    } else {
        queryString += "WHERE v.is_deleted = 0 ";
    }
    
    queryString += "ORDER BY v.id DESC";

    QSqlQuery query(m_db);
    query.prepare(queryString);
    for(auto it = bindValues.constBegin(); it != bindValues.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    if (!query.exec()) {
        qCritical() << "Failed to get all videos:" << query.lastError().text();
        return videos;
    }
    
    while(query.next()) {
        videos.append(parseVideoFromQuery(query));
    }
    return videos;
}


int VideoRepository::getVideoCount(const FilterCriteria& criteria)
{
    QString queryString = "SELECT COUNT(v.id) FROM videos v ";
    QString whereClause;
    QMap<QString, QVariant> bindValues;

    if (!criteria.searchTerm.isEmpty()) {
        queryString += "JOIN videos_fts fts ON v.id = fts.rowid ";
        whereClause += "videos_fts MATCH :searchTerm ";
        bindValues[":searchTerm"] = criteria.searchTerm + "*";
    }

    if (criteria.channelId > 0) {
        if (!whereClause.isEmpty()) whereClause += "AND ";
        whereClause += "v.channel_id = :channelId ";
        bindValues[":channelId"] = criteria.channelId;
    }

    if (!criteria.status.isEmpty() && criteria.status != "Tất cả") {
        if (!whereClause.isEmpty()) whereClause += "AND ";
        whereClause += "v.status = :status ";
        bindValues[":status"] = criteria.status;
    }

    if (!whereClause.isEmpty()) {
        queryString += "WHERE " + whereClause + "AND v.is_deleted = 0 ";
    } else {
        queryString += "WHERE v.is_deleted = 0 ";
    }
    
    QSqlQuery query(m_db);
    query.prepare(queryString);
    for(auto it = bindValues.constBegin(); it != bindValues.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    qCritical() << "Failed to get video count: " << query.lastError().text();
    return 0;
}


QList<int> VideoRepository::saveOrUpdateVideos(const QList<Video>& videos)
{
     QList<int> savedIds;
    if (!m_db.transaction()) {
        qCritical() << "Failed to start transaction:" << m_db.lastError().text();
        return savedIds;
    }

    QSqlQuery query(m_db);
    // Câu lệnh này sẽ INSERT nếu video_url chưa tồn tại, hoặc UPDATE nếu đã tồn tại
    query.prepare("INSERT INTO videos (channel_id, video_url, new_title, new_description, new_tags, new_sub_tags, status, video_date, added_date) "
                  "VALUES (:channel_id, :video_url, :new_title, :new_description, :new_tags, :new_sub_tags, :status, :video_date, :added_date) "
                  "ON CONFLICT(video_url) DO UPDATE SET "
                  "new_title = excluded.new_title, "
                  "new_description = excluded.new_description, "
                  "new_tags = excluded.new_tags, "
                  "new_sub_tags = excluded.new_sub_tags, "
                  "status = excluded.status, "
                  "video_date = excluded.video_date");

    for (const auto& videoData : videos) {
        query.bindValue(":channel_id", videoData.channelId);
        query.bindValue(":video_url", videoData.videoUrl);
        query.bindValue(":new_title", videoData.newTitle);
        query.bindValue(":new_description", videoData.newDescription);
        query.bindValue(":new_tags", videoData.newTags);
        query.bindValue(":new_sub_tags", videoData.newSubTags);
        query.bindValue(":status", videoData.status.isEmpty() ? "Pending" : videoData.status);
        query.bindValue(":video_date", videoData.videoDate.isValid() ? videoData.videoDate.toString(Qt::ISODate) : QVariant(QVariant::String));
        query.bindValue(":added_date", QDateTime::currentDateTime().toString(Qt::ISODate));

        if (!query.exec()) {
            qCritical() << "Failed to save or update video data for url" << videoData.videoUrl << " Error:" << query.lastError().text();
            m_db.rollback();
            return {};
        }

        // Lấy ID của bản ghi vừa được insert/update
        QSqlQuery idQuery(m_db);
        idQuery.prepare("SELECT id FROM videos WHERE video_url = ?");
        idQuery.addBindValue(videoData.videoUrl);
        if (idQuery.exec() && idQuery.next()) {
            savedIds.append(idQuery.value(0).toInt());
        }
    }

    if (!m_db.commit()) {
        qCritical() << "Failed to commit transaction:" << m_db.lastError().text();
        m_db.rollback();
        return {};
    }

    return savedIds;
}

bool VideoRepository::updateVideo(const Video& video)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE videos SET "
                  "channel_id = :channel_id, "
                  "video_url = :video_url, "
                  "video_date = :video_date, "
                  "new_title = :new_title, "
                  "new_description = :new_description, "
                  "new_tags = :new_tags, "
                  "new_sub_tags = :new_sub_tags, "
                  "new_playlist = :new_playlist, "
                  "new_category = :new_category, "
                  "status = :status "
                  "WHERE id = :id");
    
    query.bindValue(":channel_id", video.channelId);
    query.bindValue(":video_url", video.videoUrl);
    query.bindValue(":video_date", video.videoDate.toString(Qt::ISODate));
    query.bindValue(":new_title", video.newTitle);
    query.bindValue(":new_description", video.newDescription);
    query.bindValue(":new_tags", video.newTags);
    query.bindValue(":new_sub_tags", video.newSubTags);
    query.bindValue(":new_playlist", video.newPlaylist);
    query.bindValue(":new_category", video.newCategory);
    query.bindValue(":status", video.status);
    query.bindValue(":id", video.id);

    if (!query.exec()) {
        qCritical() << "Failed to update video info for id" << video.id << " Error:" << query.lastError().text();
        return false;
    }
    return true;
}


bool VideoRepository::videoUrlExists(const QString& url)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT 1 FROM videos WHERE video_url = ? AND is_deleted = 0");
    query.addBindValue(url);
    if (query.exec() && query.next()) {
        return true;
    }
    return false;
}

QList<Video> VideoRepository::loadPendingVideos()
{
    QList<Video> videos;
    QSqlQuery query("SELECT v.* "
                  "FROM videos v "
                  "JOIN channels c ON v.channel_id = c.id "
                  "WHERE v.status = 'Pending' AND v.is_deleted = 0 "
                  "ORDER BY c.display_order ASC, v.id ASC", m_db);

    if (!query.exec()) {
        qCritical() << "Failed to load pending videos: " << query.lastError().text();
        return videos;
    }

    while (query.next()) {
        videos.append(parseVideoFromQuery(query));
    }
    return videos;
}

QList<Video> VideoRepository::loadCompletedVideos()
{
    QList<Video> videos;
    QSqlQuery query(m_db);
    query.prepare("SELECT * FROM videos WHERE status = 'Completed' AND is_deleted = 0 ORDER BY id ASC");
    
    if (!query.exec()) {
        qCritical() << "Failed to load completed videos: " << query.lastError().text();
        return videos;
    }

    while (query.next()) {
        videos.append(parseVideoFromQuery(query));
    }
    return videos;
}

bool VideoRepository::markVideoAsUploaded(int videoId)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE videos SET status = 'Uploaded', upload_date = ? WHERE id = ?");
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    query.addBindValue(videoId);
    
    if (!query.exec()) {
        qCritical() << "Failed to mark video as uploaded for id" << videoId << " Error:" << query.lastError().text();
        return false;
    }
    return true;
}


bool VideoRepository::updateVideoDownloadedInfo(int videoId, const QString& status, const QString& filePath, const QString& originalTitle)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE videos SET status = ?, local_file_path = ?, original_title = ? WHERE id = ?");
    query.addBindValue(status);
    query.addBindValue(filePath);
    query.addBindValue(originalTitle);
    query.addBindValue(videoId);

    if (!query.exec()) {
        qCritical() << "Failed to update video downloaded info for id" << videoId << " Error:" << query.lastError().text();
        return false;
    }
    return true;
}

bool VideoRepository::updateChannelForVideos(const QList<int>& videoIds, int newChannelId)
{
    if (videoIds.isEmpty()) return true;

    m_db.transaction();
    QSqlQuery query(m_db);
    
    // Tạo chuỗi placeholder cho IN clause: (?, ?, ?)
    QStringList placeholders;
    for(int i = 0; i < videoIds.count(); ++i) {
        placeholders << "?";
    }

    query.prepare(QString("UPDATE videos SET channel_id = ? WHERE id IN (%1)").arg(placeholders.join(",")));
    query.addBindValue(newChannelId);
    for(int id : videoIds) {
        query.addBindValue(id);
    }

    if (!query.exec()) {
        qCritical() << "Failed to batch update channel for videos: " << query.lastError().text();
        m_db.rollback();
        return false;
    }
    
    return m_db.commit();
}


int VideoRepository::getUploadedVideoCount(int channelId, const QDate& startDate, const QDate& endDate)
{
    QSqlQuery query(m_db);
    QString queryString = "SELECT COUNT(*) FROM videos WHERE status = 'Uploaded' AND is_deleted = 0 AND date(upload_date) BETWEEN date(:start_date) AND date(:end_date)";
    
    if (channelId != -1) {
        queryString += " AND channel_id = :channel_id";
    }

    query.prepare(queryString);
    query.bindValue(":start_date", startDate.toString(Qt::ISODate));
    query.bindValue(":end_date", endDate.toString(Qt::ISODate));

    if (channelId != -1) {
        query.bindValue(":channel_id", channelId);
    }

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    
    qCritical() << "Failed to get uploaded video count: " << query.lastError().text();
    return 0;
}

QList<int> VideoRepository::saveVideosAndGetIds(const QList<Video>& videos)
{
    QList<int> savedIds;
    if (!m_db.transaction()) {
        qCritical() << "Failed to start transaction:" << m_db.lastError().text();
        return savedIds;
    }

    QSqlQuery query(m_db);
    query.prepare("INSERT OR REPLACE INTO videos (id, channel_id, video_url, video_date, new_title, new_description, new_tags, new_sub_tags, new_playlist, new_category, added_date, status) "
                  "VALUES ((SELECT id FROM videos WHERE video_url = :video_url), :channel_id, :video_url, :video_date, :new_title, :new_description, :new_tags, :new_sub_tags, :new_playlist, :new_category, :added_date, 'Pending')");

    for (const auto& videoData : videos) {
        query.bindValue(":channel_id", videoData.channelId);
        query.bindValue(":video_url", videoData.videoUrl);
        query.bindValue(":video_date", videoData.videoDate.toString(Qt::ISODate));
        query.bindValue(":new_title", videoData.newTitle);
        query.bindValue(":new_description", videoData.newDescription);
        query.bindValue(":new_tags", videoData.newTags);
        query.bindValue(":new_sub_tags", videoData.newSubTags);
        query.bindValue(":new_playlist", videoData.newPlaylist);
        query.bindValue(":new_category", videoData.newCategory);
        query.bindValue(":added_date", QDateTime::currentDateTime().toString(Qt::ISODate));

        if (!query.exec()) {
            qCritical() << "Failed to save video data for url" << videoData.videoUrl << " Error:" << query.lastError().text();
            m_db.rollback();
            return {};
        }

        QVariant lastId = query.lastInsertId();
        if (lastId.isValid() && lastId.toInt() != 0) {
            savedIds.append(lastId.toInt());
        } else {
            QSqlQuery idQuery(m_db);
            idQuery.prepare("SELECT id FROM videos WHERE video_url = ?");
            idQuery.addBindValue(videoData.videoUrl);
            if (idQuery.exec() && idQuery.next()) {
                savedIds.append(idQuery.value(0).toInt());
            }
        }
    }

    if (!m_db.commit()) {
        qCritical() << "Failed to commit transaction:" << m_db.lastError().text();
        m_db.rollback();
        return {};
    }

    return savedIds;
}

// Sửa lỗi: Thêm lại phương thức bị thiếu
QList<Video> VideoRepository::getVideosForDates(const QList<int>& channelIds, const QDate& startDate, const QDate& endDate)
{
    QList<Video> videos;
    if (channelIds.isEmpty()) return videos;

    QStringList idPlaceholders;
    for(int i = 0; i < channelIds.count(); ++i) {
        idPlaceholders << "?";
    }

    QSqlQuery query(m_db);
    query.prepare(QString("SELECT * FROM videos WHERE status = 'Pending' AND is_deleted = 0 AND channel_id IN (%1) AND video_date BETWEEN ? AND ?")
                          .arg(idPlaceholders.join(',')));

    for(int id : channelIds) {
        query.addBindValue(id);
    }
    query.addBindValue(startDate.toString(Qt::ISODate));
    query.addBindValue(endDate.toString(Qt::ISODate));

    if (!query.exec()) {
        qCritical() << "Failed to get videos for dates: " << query.lastError().text();
        return videos;
    }
    
    while(query.next()) {
        videos.append(parseVideoFromQuery(query));
    }
    return videos;
}

// Sửa lỗi: Thêm lại phương thức bị thiếu
bool VideoRepository::updateVideoUploadInfo(const Video& video)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE videos SET new_title = ?, new_description = ?, new_tags = ?, new_sub_tags = ?, new_playlist = ?, new_category = ? WHERE id = ?");
    query.addBindValue(video.newTitle);
    query.addBindValue(video.newDescription);
    query.addBindValue(video.newTags);
    query.addBindValue(video.newSubTags);
    query.addBindValue(video.newPlaylist);
    query.addBindValue(video.newCategory);
    query.addBindValue(video.id);

    if (!query.exec()) {
        qCritical() << "Failed to update video upload info for id" << video.id << " Error:" << query.lastError().text();
        return false;
    }
    return true;
}


bool VideoRepository::deleteVideos(const QList<int>& videoIds, bool permanently)
{
    if(videoIds.isEmpty()) return true;

    if (permanently) {
        // This is now handled by clearDeletedVideos
        qWarning() << "Permanent delete called from deleteVideos, which is deprecated. Use clearDeletedVideos instead.";
        return false;
    }
    
    // Soft delete
    m_db.transaction();
    QSqlQuery query(m_db);
    QStringList placeholders;
    for(int i = 0; i < videoIds.count(); ++i) placeholders << "?";
    
    query.prepare(QString("UPDATE videos SET is_deleted = 1, deletion_date = ? WHERE id IN (%1)").arg(placeholders.join(",")));
    query.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));
    for(int id : videoIds) query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Failed to soft-delete videos:" << query.lastError().text();
        m_db.rollback();
        return false;
    }
    return m_db.commit();
}

bool VideoRepository::restoreVideos(const QList<int>& videoIds)
{
    if(videoIds.isEmpty()) return true;

    m_db.transaction();
    QSqlQuery query(m_db);
    QStringList placeholders;
    for(int i = 0; i < videoIds.count(); ++i) placeholders << "?";

    query.prepare(QString("UPDATE videos SET is_deleted = 0, deletion_date = NULL WHERE id IN (%1)").arg(placeholders.join(",")));
    for(int id : videoIds) query.addBindValue(id);
    
    if (!query.exec()) {
        qCritical() << "Failed to restore videos:" << query.lastError().text();
        m_db.rollback();
        return false;
    }
    return m_db.commit();
}

int VideoRepository::getSoftDeletedVideoCount()
{
    QSqlQuery query("SELECT COUNT(*) FROM videos WHERE is_deleted = 1", m_db);
    if(query.exec() && query.next()){
        return query.value(0).toInt();
    }
    qCritical() << "Failed to count soft-deleted videos:" << query.lastError().text();
    return 0;
}

int VideoRepository::clearDeletedVideos()
{
    QSqlQuery query("DELETE FROM videos WHERE is_deleted = 1", m_db);
    if(query.exec()){
        return query.numRowsAffected();
    }
    qCritical() << "Failed to clear deleted videos:" << query.lastError().text();
    return -1;
}

QMap<QString, QList<Video>> VideoRepository::findDuplicateVideos()
{
    QMap<QString, QList<Video>> duplicates;
    // Bước 1: Tìm các URL bị trùng lặp
    QSqlQuery urlQuery("SELECT video_url FROM videos WHERE is_deleted = 0 GROUP BY video_url HAVING COUNT(id) > 1", m_db);
    if(!urlQuery.exec()) {
        qCritical() << "Failed to find duplicate URLs:" << urlQuery.lastError().text();
        return duplicates;
    }

    QStringList duplicateUrls;
    while(urlQuery.next()) {
        duplicateUrls.append(urlQuery.value(0).toString());
    }

    if (duplicateUrls.isEmpty()) {
        return duplicates;
    }

    // Bước 2: Lấy thông tin chi tiết cho từng video có URL trùng lặp
    for(const QString& url : duplicateUrls) {
        QSqlQuery videoQuery(m_db);
        videoQuery.prepare("SELECT * FROM videos WHERE video_url = ? AND is_deleted = 0");
        videoQuery.addBindValue(url);
        if (videoQuery.exec()) {
            QList<Video> videoGroup;
            while(videoQuery.next()) {
                videoGroup.append(parseVideoFromQuery(videoQuery));
            }
            duplicates[url] = videoGroup;
        }
    }
    return duplicates;
}


Video VideoRepository::parseVideoFromQuery(QSqlQuery& query)
{
    Video v;
    const QSqlRecord record = query.record();
    v.id = record.value("id").toInt();
    v.channelId = record.value("channel_id").toInt();
    v.videoUrl = record.value("video_url").toString();
    v.originalTitle = record.value("original_title").toString();
    v.localFilePath = record.value("local_file_path").toString();
    v.newTitle = record.value("new_title").toString();
    v.newDescription = record.value("new_description").toString();
    v.newTags = record.value("new_tags").toString();
    v.newSubTags = record.value("new_sub_tags").toString();
    v.newPlaylist = record.value("new_playlist").toString();
    v.newCategory = record.value("new_category").toString();
    v.videoDate = QDate::fromString(record.value("video_date").toString(), Qt::ISODate);
    v.status = record.value("status").toString();
    v.addedDate = record.value("added_date").toString();
    v.uploadDate = record.value("upload_date").toString();
    return v;
}

