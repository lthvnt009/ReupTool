// Vị trí: /src/widgets/channelstatisticswidget.h
// Phiên bản: 1.3 (Xử lý thay đổi kích thước và văn bản dài)

#pragma once

#include <QWidget>
#include "../models.h"

// Forward declaration
class QResizeEvent;

namespace Ui {
class ChannelStatisticsWidget;
}

class IChannelRepository;
class IVideoRepository;

class ChannelStatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelStatisticsWidget(IChannelRepository* channelRepo, IVideoRepository* videoRepo, QWidget *parent = nullptr);
    ~ChannelStatisticsWidget();

public slots:
    void updateForChannel(const Channel& channel);
    void updateForMultipleChannels(int count);
    void clearStatistics();

protected:
    // Ghi đè sự kiện thay đổi kích thước để cập nhật lại văn bản
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onCalculateClicked();
    void onTimeRangeChanged(int index);

private:
    // Hàm helper để cập nhật hiển thị tên kênh (cắt bớt nếu cần)
    void updateChannelNameDisplay();

    Ui::ChannelStatisticsWidget *ui;
    IChannelRepository* m_channelRepo;
    IVideoRepository* m_videoRepo;
    Channel m_currentChannel;
};

