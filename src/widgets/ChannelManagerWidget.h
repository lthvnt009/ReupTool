// Vị trí: /src/widgets/channelmanagerwidget.h
// Phiên bản: 2.3 (Thêm getter cho splitter)

#pragma once

#include <QWidget>

// Forward declaration
namespace Ui {
class ChannelManagerWidget;
}
class ChannelToolbarWidget;
class ChannelListWidget;
class ChannelStatisticsWidget;
class IChannelRepository;
class IVideoRepository;
class QSplitter; // Thêm forward declaration

class ChannelManagerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelManagerWidget(
        IChannelRepository* channelRepo,
        IVideoRepository* videoRepo,
        QWidget *parent = nullptr
    );
    ~ChannelManagerWidget();

    // Các phương thức getter cho các widget con
    ChannelToolbarWidget* getToolbarWidget() const;
    ChannelListWidget* getListWidget() const;
    ChannelStatisticsWidget* getStatisticsWidget() const;
    QSplitter* getSplitter() const; // Thêm getter cho splitter

private:
    Ui::ChannelManagerWidget *ui;
    ChannelStatisticsWidget* m_statisticsWidget;
};
