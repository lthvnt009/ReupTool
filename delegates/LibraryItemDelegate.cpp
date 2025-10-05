// Vị trí: /src/delegates/LibraryItemDelegate.cpp
// Phiên bản: 1.2 (Thêm tín hiệu editClicked)

#include "libraryitemdelegate.h"
#include "../models/videolibrarymodel.h"
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

void LibraryItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() == VideoLibraryModel::Tags) {
        QStringList tags = index.data().toString().split(", ", Qt::SkipEmptyParts);
        if (tags.isEmpty()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        painter->save();
        QStyledItemDelegate::paint(painter, option, index); // Vẽ nền trước

        // Logic vẽ "viên thuốc"
        QFontMetrics fm(option.font);
        int x = option.rect.x() + 5;
        int y = option.rect.y() + (option.rect.height() - fm.height() - 4) / 2;

        for (const QString& tag : tags) {
            int width = fm.horizontalAdvance(tag) + 10;
            if (x + width > option.rect.right()) break;

            QRectF tagRect(x, y, width, fm.height() + 4);
            painter->setBrush(QColor(220, 220, 220));
            painter->setPen(Qt::NoPen);
            painter->drawRoundedRect(tagRect, 8, 8);

            painter->setPen(Qt::black);
            painter->drawText(tagRect, Qt::AlignCenter, tag);
            x += width + 5;
        }
        painter->restore();
    }
    else if (index.column() == VideoLibraryModel::Actions) {
        QStyledItemDelegate::paint(painter, option, index); // Vẽ nền

        int buttonWidth = 24;
        int spacing = 5;
        int totalWidth = buttonWidth * 2 + spacing;
        int x = option.rect.x() + (option.rect.width() - totalWidth) / 2;
        int y = option.rect.y() + (option.rect.height() - buttonWidth) / 2;

        // Vẽ nút Sửa
        QStyleOptionButton editButton;
        editButton.rect = QRect(x, y, buttonWidth, buttonWidth);
        editButton.text = "✏️";
        QApplication::style()->drawControl(QStyle::CE_PushButton, &editButton, painter);

        // Vẽ nút Xóa
        QStyleOptionButton deleteButton;
        deleteButton.rect = QRect(x + buttonWidth + spacing, y, buttonWidth, buttonWidth);
        deleteButton.text = "🗑️";
        QApplication::style()->drawControl(QStyle::CE_PushButton, &deleteButton, painter);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

bool LibraryItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress && index.column() == VideoLibraryModel::Actions) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        
        int buttonWidth = 24;
        int spacing = 5;
        int totalWidth = buttonWidth * 2 + spacing;
        int x = option.rect.x() + (option.rect.width() - totalWidth) / 2;
        int y = option.rect.y() + (option.rect.height() - buttonWidth) / 2;

        QRect editButtonRect(x, y, buttonWidth, buttonWidth);
        QRect deleteButtonRect(x + buttonWidth + spacing, y, buttonWidth, buttonWidth);

        if (editButtonRect.contains(mouseEvent->pos())) {
            emit editClicked(index);
            return true;
        }
        if (deleteButtonRect.contains(mouseEvent->pos())) {
            emit deleteClicked(index);
            return true;
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

