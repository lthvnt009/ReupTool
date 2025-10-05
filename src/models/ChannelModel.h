// Vị trí: /src/models/channelmodel.h
// Phiên bản: 1.1 (Sửa lỗi Drag & Drop - Hoàn thiện)
// Mô tả: Lớp model tùy chỉnh cho bảng kênh, hỗ trợ Drag & Drop.

#pragma once

#include <QStandardItemModel>

class ChannelModel : public QStandardItemModel
{
    Q_OBJECT
public:
    using QStandardItemModel::QStandardItemModel;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    
    // *** SỬA LỖI (Bổ sung) ***
    // Khai báo rằng model này hỗ trợ hành động di chuyển.
    Qt::DropActions supportedDropActions() const override;


signals:
    void orderChanged();
};
