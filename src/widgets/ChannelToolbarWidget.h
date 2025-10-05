// Vị trí: /src/widgets/channeltoolbarwidget.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Widget thanh công cụ cho tab Quản lý kênh.

#pragma once

#include <QWidget>

namespace Ui {
class ChannelToolbarWidget;
}

class ChannelToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelToolbarWidget(QWidget *parent = nullptr);
    ~ChannelToolbarWidget();

    Ui::ChannelToolbarWidget* getUi() const;

private:
    Ui::ChannelToolbarWidget *ui;
};
