// Vị trí: /src/models/downloadqueuemodel.h
// Phiên bản: 1.3 (Thêm getter)
// Mô tả: Định nghĩa QAbstractTableModel để hiển thị hàng đợi tải.

#pragma once

#include <QAbstractTableModel>
#include <QList>
#include "../models.h"

class DownloadQueueModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DownloadQueueModel(QObject *parent = nullptr);

    void setQueue(QList<DownloadItem> *queue);
    void updateQueue();
    void updateItem(int row);

    // Cung cấp quyền truy cập chỉ đọc vào hàng đợi
    const QList<DownloadItem>* getQueue() const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QList<DownloadItem> *m_queue;
};
