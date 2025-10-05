// Vị trí: /src/models/VideoLibraryModel.h
// Phiên bản: 1.3 (Thêm getters)
// Mô tả: Model dữ liệu cho bảng Thư viện.

#pragma once

#include <QAbstractTableModel>
#include <QList>
#include <QHash>
#include <QSet>
#include "../models.h"

class VideoLibraryModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        Check, ID, Channel, Link, NewTitle, Status, VideoDate, UploadDate, Tags, Actions, ColumnCount
    };

    explicit VideoLibraryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setVideos(const QList<Video>& videos);
    void setChannelMap(const QHash<int, QString>& channelMap);
    QList<int> getCheckedVideoIds() const;
    void clearCheckedState();

    // Các hàm getter mới để sửa lỗi build
    Video getVideo(int row) const;
    bool isChecked(int row) const;

signals:
    void checkedCountChanged(int count);

private:
    QList<Video> m_videos;
    QHash<int, QString> m_channelMap;
    QSet<int> m_checkedStates; // Sử dụng Set để lưu ID
};

