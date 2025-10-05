// Vị trí: /src/models/VideoLibraryModel.cpp
// Phiên bản: 1.3 (Thêm getters)

#include "videolibrarymodel.h"
#include <QBrush>
#include <QColor>

VideoLibraryModel::VideoLibraryModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int VideoLibraryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_videos.count();
}

int VideoLibraryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return ColumnCount;
}

QVariant VideoLibraryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_videos.count())
        return QVariant();

    const Video &video = m_videos[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case ID: return video.id;
            case Channel: return m_channelMap.value(video.channelId, "Không rõ");
            case Link: return video.videoUrl;
            case NewTitle: return video.newTitle.isEmpty() ? video.originalTitle : video.newTitle;
            case Status: return video.status;
            case VideoDate: return video.videoDate.toString("dd/MM/yyyy");
            case UploadDate: return video.uploadDate.isEmpty() ? "" : QDateTime::fromString(video.uploadDate, Qt::ISODate).toString("dd/MM/yyyy");
            case Tags: return video.newTags;
            default: break;
        }
    }

    if (role == Qt::CheckStateRole && index.column() == Check) {
        return m_checkedStates.contains(video.id) ? Qt::Checked : Qt::Unchecked;
    }
    
    if (role == Qt::ForegroundRole && index.column() == Status) {
        if (video.status == "Uploaded") return QBrush(QColor("green"));
        if (video.status == "Completed") return QBrush(QColor("blue"));
        if (video.status == "Failed") return QBrush(QColor("red"));
    }


    return QVariant();
}

QVariant VideoLibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case Check: return "";
            case Channel: return "Kênh";
            case NewTitle: return "Tiêu đề";
            case Status: return "Trạng thái";
            case VideoDate: return "Ngày Video";
            case UploadDate: return "Ngày Upload";
            case Tags: return "Tags";
            case Actions: return "Hành động";
            default: break;
        }
    }
    return QVariant();
}

bool VideoLibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_videos.count())
        return false;

    if (role == Qt::CheckStateRole && index.column() == Check) {
        int videoId = m_videos[index.row()].id;
        if (value.toInt() == Qt::Checked) {
            m_checkedStates.insert(videoId);
        } else {
            m_checkedStates.remove(videoId);
        }
        emit dataChanged(index, index, {role});
        emit checkedCountChanged(m_checkedStates.count());
        return true;
    }

    return QAbstractTableModel::setData(index, value, role);
}

Qt::ItemFlags VideoLibraryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    if (index.column() == Check) {
        return defaultFlags | Qt::ItemIsUserCheckable;
    }
    return defaultFlags;
}


void VideoLibraryModel::setVideos(const QList<Video>& videos)
{
    beginResetModel();
    m_videos = videos;
    clearCheckedState();
    endResetModel();
}

void VideoLibraryModel::setChannelMap(const QHash<int, QString>& channelMap)
{
    m_channelMap = channelMap;
}

QList<int> VideoLibraryModel::getCheckedVideoIds() const
{
    return m_checkedStates.values();
}

void VideoLibraryModel::clearCheckedState()
{
    if (m_checkedStates.isEmpty()) return;
    m_checkedStates.clear();
    emit checkedCountChanged(0);
    // Thông báo cho view cập nhật lại cột checkbox
    emit dataChanged(index(0, Check), index(m_videos.count() - 1, Check), {Qt::CheckStateRole});
}

Video VideoLibraryModel::getVideo(int row) const
{
    if (row >= 0 && row < m_videos.count()) {
        return m_videos.at(row);
    }
    return Video(); // Trả về video không hợp lệ
}

bool VideoLibraryModel::isChecked(int row) const
{
    if (row >= 0 && row < m_videos.count()) {
        return m_checkedStates.contains(m_videos.at(row).id);
    }
    return false;
}

