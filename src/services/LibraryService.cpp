// Vị trí: /src/services/LibraryService.cpp
// Phiên bản: 1.8 (Tối ưu hóa Signal)

#include "libraryservice.h"
#include "../repositories/ivideorepository.h"
#include "../repositories/ichannelrepository.h"
#include "../services/fileimportservice.h"
#include <QtConcurrent>
#include <QDebug>
#include "xlsxdocument.h"

LibraryService::LibraryService(IVideoRepository* videoRepo, IChannelRepository* channelRepo, QObject *parent)
    : QObject(parent), m_videoRepo(videoRepo), m_channelRepo(channelRepo)
{
}

void LibraryService::requestData(const FilterCriteria& criteria)
{
    if (!m_videoRepo) return;

    (void)QtConcurrent::run([this, criteria]() {
        int totalItems = m_videoRepo->getVideoCount(criteria);
        QList<Video> videos = m_videoRepo->getVideos(criteria);
        
        QMetaObject::invokeMethod(this, [this, videos, totalItems]{
            emit videosReady(videos);
            emit countReady(totalItems, videos.count());
        }, Qt::QueuedConnection);
    });
}

void LibraryService::requestImport(const QString& filePath)
{
    if (!m_videoRepo || !m_channelRepo) return;

    (void)QtConcurrent::run([this, filePath]() {
        FileImportService importService;
        
        QMap<QString, int> channelNameToIdMap;
        for (const auto& ch : m_channelRepo->loadAllChannels()) {
            channelNameToIdMap[ch.name] = ch.id;
        }

        QList<Video> videosToImport = importService.importVideosFromXlsx(filePath, channelNameToIdMap);

        if (videosToImport.isEmpty()) {
            emit taskFinished(false, "Lỗi Nhập File", "Không tìm thấy video hợp lệ nào trong file hoặc đã xảy ra lỗi đọc file.");
            return;
        }
        
        QList<int> importedIds = m_videoRepo->saveOrUpdateVideos(videosToImport);
        
        emit taskFinished(true, "Hoàn tất", QString("Đã nhập và cập nhật %1 video thành công.").arg(importedIds.count()));
        if(!importedIds.isEmpty()) emit videosModified(importedIds);
    });
}


void LibraryService::requestExport(const QString& filePath, const FilterCriteria& criteria)
{
     if (!m_videoRepo || !m_channelRepo) return;

    (void)QtConcurrent::run([this, filePath, criteria]() {
        QList<Video> videosToExport = m_videoRepo->getAllVideos(criteria);
        QList<Channel> channels = m_channelRepo->loadAllChannels();
        QMap<int, QString> channelIdToNameMap;
        for(const auto& ch : channels) {
            channelIdToNameMap[ch.id] = ch.name;
        }

        QXlsx::Document xlsx;
        xlsx.renameSheet("Sheet1", "Thư viện Video");

        const QStringList headers = { "ID", "Kênh", "Link Video", "Tiêu đề mới", "Mô tả mới", "Tags mới", "Tags con mới", "Trạng thái", "Ngày video" };
        for (int i = 0; i < headers.count(); ++i) {
            xlsx.write(1, i + 1, headers[i]);
        }
        
        for (int i = 0; i < videosToExport.count(); ++i) {
            const auto& video = videosToExport[i];
            xlsx.write(i + 2, 1, video.id);
            xlsx.write(i + 2, 2, channelIdToNameMap.value(video.channelId, "Không rõ"));
            xlsx.write(i + 2, 3, video.videoUrl);
            xlsx.write(i + 2, 4, video.newTitle);
            xlsx.write(i + 2, 5, video.newDescription);
            xlsx.write(i + 2, 6, video.newTags);
            xlsx.write(i + 2, 7, video.newSubTags);
            xlsx.write(i + 2, 8, video.status);
            xlsx.write(i + 2, 9, video.videoDate.toString(Qt::ISODate));
        }

        if (xlsx.saveAs(filePath)) {
            emit taskFinished(true, "Xuất thành công", QString("Đã xuất %1 video ra file:\n%2").arg(videosToExport.count()).arg(filePath));
        } else {
            emit taskFinished(false, "Xuất thất bại", "Không thể lưu file Excel.");
        }
    });
}

void LibraryService::requestBatchEditTags(const QList<int>& videoIds, int action, const QString& tag1, const QString& tag2)
{
    if (!m_videoRepo) return;

    (void)QtConcurrent::run([this, videoIds, action, tag1, tag2]() {
        bool changed = false;
        for(int id : videoIds) {
            Video video = m_videoRepo->getVideoById(id);
            if(video.id == -1) continue;

            QStringList tags = video.newTags.split(",", Qt::SkipEmptyParts);
            for(auto& tag : tags) tag = tag.trimmed();
            QSet<QString> tagSet = QSet<QString>(tags.begin(), tags.end());
            int originalSize = tagSet.size();

            switch(action) {
                case 0: tagSet.insert(tag1); break;
                case 1: tagSet.remove(tag1); break;
                case 2: if(tagSet.contains(tag1)) { tagSet.remove(tag1); tagSet.insert(tag2); } break;
            }
            if (tagSet.size() != originalSize || action == 2) {
                video.newTags = QStringList(tagSet.values()).join(", ");
                if (m_videoRepo->updateVideo(video)) {
                    changed = true;
                }
            }
        }
        emit batchEditCompleted(true, "Chỉnh sửa tags hàng loạt hoàn tất!");
        if (changed) emit videosModified(videoIds);
    });
}

void LibraryService::requestBatchChangeChannel(const QList<int>& videoIds, int newChannelId)
{
    if(!m_videoRepo) return;
    (void)QtConcurrent::run([this, videoIds, newChannelId](){
        bool success = m_videoRepo->updateChannelForVideos(videoIds, newChannelId);
        if (success) {
            emit batchEditCompleted(true, QString("Đã chuyển %1 video sang kênh mới thành công.").arg(videoIds.count()));
            emit videosModified(videoIds);
        } else {
            emit batchEditCompleted(false, "Đã có lỗi xảy ra khi chuyển kênh.");
        }
    });
}

void LibraryService::requestUpdateVideo(const Video& video)
{
    if(!m_videoRepo) return;
    (void)QtConcurrent::run([this, video](){
        bool success = m_videoRepo->updateVideo(video);
        if (success) {
            requestData(FilterCriteria());
            emit videosModified({video.id});
        }
    });
}


void LibraryService::requestDelete(const QList<int>& videoIds, bool deleteFiles)
{
    if (!m_videoRepo) return;

    Q_UNUSED(deleteFiles);

    (void)QtConcurrent::run([this, videoIds]() {
        bool success = m_videoRepo->deleteVideos(videoIds, false);
        if(success) {
            m_lastDeletedVideoIds = videoIds;
            emit deleteCompleted(true, QString("Đã xóa %1 video.").arg(videoIds.count()), videoIds);
            emit videosModified(videoIds);
        } else {
            emit deleteCompleted(false, "Xóa video thất bại.", {});
        }
    });
}

void LibraryService::requestRestoreLastDeleted()
{
    if (!m_videoRepo || m_lastDeletedVideoIds.isEmpty()) {
        emit restoreCompleted(false, "Không có gì để hoàn tác.");
        return;
    };

    (void)QtConcurrent::run([this]() {
        QList<int> idsToRestore = m_lastDeletedVideoIds; // Sao chép lại để tránh race condition
        bool success = m_videoRepo->restoreVideos(idsToRestore);
        if(success) {
            emit restoreCompleted(true, QString("Đã hoàn tác, khôi phục %1 video.").arg(idsToRestore.count()));
            m_lastDeletedVideoIds.clear();
            emit videosModified(idsToRestore);
        } else {
            emit restoreCompleted(false, "Hoàn tác thất bại.");
        }
    });
}

void LibraryService::requestCleanupInfo()
{
    if (!m_videoRepo) return;
    (void)QtConcurrent::run([this](){
        int count = m_videoRepo->getSoftDeletedVideoCount();
        emit cleanupInfoReady(count);
    });
}

void LibraryService::requestCleanup()
{
    if (!m_videoRepo) return;
    (void)QtConcurrent::run([this](){
        int affectedRows = m_videoRepo->clearDeletedVideos();
        if (affectedRows >= 0) {
            emit cleanupCompleted(true, QString("Đã dọn dẹp và xóa vĩnh viễn %1 video.").arg(affectedRows));
        } else {
            emit cleanupCompleted(false, "Quá trình dọn dẹp đã xảy ra lỗi.");
        }
    });
}

void LibraryService::requestFindDuplicates()
{
    if (!m_videoRepo) return;
    (void)QtConcurrent::run([this](){
        QMap<QString, QList<Video>> duplicates = m_videoRepo->findDuplicateVideos();
        emit duplicatesFound(duplicates);
    });
}

