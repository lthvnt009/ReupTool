// Vị trí: /src/models/channelmodel.cpp
// Phiên bản: 1.8 (Sửa lỗi Build khi kéo thả nhiều mục)
// Mô tả: Triển khai logic cho ChannelModel.

#include "channelmodel.h"
#include <QMimeData>
#include <QDataStream>
#include <QIODevice>
#include <algorithm>
#include <functional>

Qt::ItemFlags ChannelModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QStandardItemModel::flags(index);

    if (index.isValid()) {
        // Cho phép Kéo (Drag) và Thả (Drop) trên một hàng dữ liệu cụ thể.
        return defaultFlags | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
    }
    
    // Cho phép Thả (Drop) trên khu vực trống của bảng (để thả vào cuối).
    return defaultFlags | Qt::ItemIsDropEnabled;
}

Qt::DropActions ChannelModel::supportedDropActions() const
{
    // Chỉ cho phép hành động di chuyển (Move), không cho phép sao chép (Copy).
    return Qt::MoveAction;
}

bool ChannelModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(column);
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("application/x-qabstractitemmodeldatalist"))
        return false;
    
    int destRow = row;
    if (parent.isValid()) {
        destRow = parent.row();
    }

    // Giải mã dữ liệu để lấy tất cả các hàng nguồn
    QByteArray encodedData = data->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);
    QList<int> sourceRows;
    while (!stream.atEnd()) {
        int r, c;
        QMap<int, QVariant> v;
        stream >> r >> c >> v;
        // Chỉ lấy chỉ số hàng một lần cho mỗi hàng
        if (c == 0 && !sourceRows.contains(r)) {
             sourceRows.append(r);
        }
    }
    // Sắp xếp các hàng nguồn theo thứ tự giảm dần để xóa an toàn
    std::sort(sourceRows.begin(), sourceRows.end(), [](int a, int b) { return a > b; });
    
    if (sourceRows.isEmpty()) {
        return false;
    }

    // Lấy ra tất cả các hàng nguồn
    QList<QList<QStandardItem *>> takenRows;
    for (int sourceRow : sourceRows) {
        takenRows.prepend(takeRow(sourceRow)); // Thêm vào đầu để giữ đúng thứ tự ban đầu
    }

    // Xác định hàng để chèn vào
    int insertRow = destRow;
    if (insertRow == -1) {
        // Thả vào vùng trống, chèn vào cuối
        insertRow = rowCount();
    } else {
        // Điều chỉnh chỉ số hàng chèn dựa trên vị trí của các hàng đã xóa
        int numRowsRemovedBeforeDest = 0;
        for (int sourceRow : sourceRows) {
            if (sourceRow < destRow) {
                numRowsRemovedBeforeDest++;
            }
        }
        insertRow -= numRowsRemovedBeforeDest;
    }

    // Chèn các hàng đã lấy vào vị trí đích
    for (const auto& rowItems : takenRows) {
        // SỬA LỖI: Sử dụng this-> để gọi phương thức của lớp,
        // tránh nhầm lẫn với biến cục bộ 'insertRow'.
        this->insertRow(insertRow, rowItems);
        insertRow++;
    }

    emit orderChanged();
    return true;
}
