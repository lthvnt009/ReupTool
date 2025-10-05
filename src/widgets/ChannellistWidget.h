// Vị trí: /src/widgets/channellistwidget.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Widget quản lý danh sách kênh (cột trái).

#pragma once

#include <QWidget>

namespace Ui {
class ChannelListWidget;
}

class ChannelListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelListWidget(QWidget *parent = nullptr);
    ~ChannelListWidget();

    Ui::ChannelListWidget* getUi() const;

private:
    Ui::ChannelListWidget *ui;
};
