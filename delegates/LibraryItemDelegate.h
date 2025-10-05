// Vị trí: /src/delegates/LibraryItemDelegate.h
// Phiên bản: 1.2 (Thêm tín hiệu editClicked)
// Mô tả: Delegate tùy chỉnh để vẽ các item trong bảng Thư viện.

#pragma once

#include <QStyledItemDelegate>

class LibraryItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    
    // Ghi đè để xử lý sự kiện click chuột trên các nút tùy chỉnh
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void deleteClicked(const QModelIndex& index);
    void editClicked(const QModelIndex& index); // Tín hiệu mới
};

