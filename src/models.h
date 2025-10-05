// Vị trí: /src/models.h
// Phiên bản: 2.1 (Thêm model cho Cài đặt)

#pragma once

#include <QString>
#include <QDate>
#include <QUuid>
#include "models/FilterCriteria.h"

// Struct để chứa thông tin một kênh
struct Channel {
    int id = -1;
    QString name;
    QString link;
    int displayOrder = 0;
    bool isHidden = false;
};

// Struct để chứa thông tin một video
struct Video {
    int id = -1;
    int channelId = -1;
    QDate videoDate;
    QString videoUrl;
    QString originalTitle;
    QString status; // "Pending", "Downloading", "Failed", "Completed", "Uploaded"
    QString addedDate;
    QString uploadDate;
    QString localFilePath;
    QString newTitle;
    QString newDescription;
    QString newTags;
    QString newSubTags;
    QString newPlaylist;
    QString newCategory;
};

// *** CÁC STRUCT CHO TAB TẢI VỀ ***

// Enum cho trạng thái tải
enum class DownloadStatus {
    Queued,
    Downloading,
    Paused,
    Stopped,
    Completed,
    Error,
    Skipped
};

// Struct chứa các tùy chọn tải được lấy từ UI
struct DownloadOptions {
    QString downloadType = "Video";
    QString format = "MP4";
    QString quality = "Best";
    QString savePath;
    QString thumbnailFormat = "Bỏ qua";
    QString subtitleLang = "Bỏ qua";
    bool downloadPlaylist = false;
    bool writeDescription = false;
    bool cutVideo = false;
    QString cutTimeRange;
    bool useCustomCommand = false;
    QString customCommand;
    QString outputTemplate;

    QStringList sponsorBlockRemove;
    QStringList sponsorBlockMark;
};


// Struct chính đại diện cho một mục trong hàng đợi tải
struct DownloadItem {
    QString uuid = QUuid::createUuid().toString();
    QString url;
    QString title;
    DownloadStatus status = DownloadStatus::Queued;
    int progress = 0;
    qint64 size = 0;
    QString speed;
    QString timeRemaining;
    QString localFilePath;
    QString errorMessage;
    QString logOutput;

    DownloadOptions options;
    int dbVideoId = -1;
};

// *** CÁC STRUCT MỚI CHO TAB CÀI ĐẶT ***
struct ApiKey {
    QString id = QUuid::createUuid().toString();
    QString platform = "YouTube"; // Mở rộng trong tương lai
    QString alias;
    QString keyValue;
    bool isValid = false; // Sẽ được cập nhật sau khi kiểm tra
};

struct Proxy {
    QString id = QUuid::createUuid().toString();
    QString host;
    int port = 0;
    QString username;
    QString password;
};


// Hàm tiện ích
inline QString statusToString(DownloadStatus status) {
    switch (status) {
        case DownloadStatus::Queued: return "Đang chờ";
        case DownloadStatus::Downloading: return "Đang tải";
        case DownloadStatus::Paused: return "Tạm dừng";
        case DownloadStatus::Stopped: return "Đã dừng";
        case DownloadStatus::Completed: return "Hoàn thành";
        case DownloadStatus::Error: return "Lỗi";
        case DownloadStatus::Skipped: return "Đã bỏ qua";
        default: return "Không rõ";
    }
}
