// Vị trí: /src/models/downloadqueuemodel.cpp
// Phiên bản: 1.3 (Thêm getter)
// Mô tả: Triển khai logic cho DownloadQueueModel.

#include "downloadqueuemodel.h"
#include <QBrush>
#include <QColor>

DownloadQueueModel::DownloadQueueModel(QObject *parent)
    : QAbstractTableModel(parent)
    , m_queue(nullptr)
{
}

void DownloadQueueModel::setQueue(QList<DownloadItem> *queue)
{
    beginResetModel();
    m_queue = queue;
    endResetModel();
}

void DownloadQueueModel::updateQueue()
{
    beginResetModel();
    endResetModel();
}

void DownloadQueueModel::updateItem(int row)
{
    if (!m_queue || row < 0 || row >= m_queue->size()) return;
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

const QList<DownloadItem>* DownloadQueueModel::getQueue() const
{
    return m_queue;
}

int DownloadQueueModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_queue ? m_queue->size() : 0;
}

int DownloadQueueModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 8; // #, Tên video, Trạng thái, Định dạng, Kích thước, Tiến trình, Tốc độ, Thời gian tải
}

QVariant DownloadQueueModel::data(const QModelIndex &index, int role) const
{
    if (!m_queue || !index.isValid() || index.row() >= m_queue->size())
        return QVariant();

    const DownloadItem &item = (*m_queue)[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return index.row() + 1;
            case 1: return item.title;
            case 2: return statusToString(item.status);
            case 3: return item.options.format;
            case 4: return item.size > 0 ? QString::number(item.size / (1024.0 * 1024.0), 'f', 2) + " MB" : "N/A";
            case 5: return item.progress; // Trả về số để delegate xử lý
            case 6: return item.speed;
            case 7: return item.timeRemaining;
        }
    }
    
    if (role == Qt::UserRole) {
        if(index.column() == 5) return item.progress;
    }

    if (role == Qt::ForegroundRole) {
        if (item.status == DownloadStatus::Error) {
            return QBrush(QColor("red"));
        }
        if (item.status == DownloadStatus::Completed) {
            return QBrush(QColor("green"));
        }
    }
    
    return QVariant();
}

QVariant DownloadQueueModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
        case 0: return "#";
        case 1: return "Tên video";
        case 2: return "Trạng thái";
        case 3: return "Định dạng";
        case 4: return "Kích thước";
        case 5: return "Tiến trình";
        case 6: return "Tốc độ";
        case 7: return "Thời gian tải";
        default: return QVariant();
    }
}
