// Vị trí: /src/widgets/channelstatisticswidget.cpp
// Phiên bản: 1.6 (Cải tiến giao diện theo yêu cầu)

#include "channelstatisticswidget.h"
#include "ui_channelstatisticswidget.h"
#include "../repositories/ichannelrepository.h"
#include "../repositories/ivideorepository.h"
#include <QDate>
#include <QFontMetrics>
#include <QResizeEvent>

ChannelStatisticsWidget::ChannelStatisticsWidget(IChannelRepository* channelRepo, IVideoRepository* videoRepo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelStatisticsWidget),
    m_channelRepo(channelRepo),
    m_videoRepo(videoRepo)
{
    ui->setupUi(this);

    ui->timeRangeComboBox->addItem("Tuần này", 0);
    ui->timeRangeComboBox->addItem("Tháng này", 1);
    ui->timeRangeComboBox->addItem("Năm nay", 2);
    ui->timeRangeComboBox->addItem("Tùy chỉnh", 3);

    onTimeRangeChanged(0);
    ui->timeRangeComboBox->setCurrentIndex(0);

    connect(ui->calculateButton, &QPushButton::clicked, this, &ChannelStatisticsWidget::onCalculateClicked);
    connect(ui->timeRangeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChannelStatisticsWidget::onTimeRangeChanged);

    // Xóa thông tin ban đầu
    clearStatistics();
}

ChannelStatisticsWidget::~ChannelStatisticsWidget()
{
    delete ui;
}

void ChannelStatisticsWidget::updateForChannel(const Channel& channel)
{
    m_currentChannel = channel;
    if (m_currentChannel.id != -1) {
        // Cập nhật hiển thị tên kênh và kích hoạt panel
        updateChannelNameDisplay();
        ui->groupBox->setEnabled(true);
        onCalculateClicked();
    } else {
        // Nếu không có kênh hợp lệ, xóa thông tin
        clearStatistics();
    }
}

void ChannelStatisticsWidget::updateForMultipleChannels(int count)
{
    m_currentChannel.id = -1; // Đánh dấu là không có kênh cụ thể nào được chọn
    ui->channelNameLabel->setText(QString("%1 kênh đang được chọn").arg(count));
    ui->channelNameLabel->setToolTip(""); // Xóa tooltip
    ui->resultLabel->setText("Vui lòng chỉ chọn một kênh để xem thống kê.");
    ui->groupBox->setEnabled(false);
}

void ChannelStatisticsWidget::clearStatistics()
{
    m_currentChannel.id = -1;
    ui->channelNameLabel->setText("Vui lòng chọn một kênh");
    ui->channelNameLabel->setToolTip("");
    ui->resultLabel->setText("Kết quả sẽ được hiển thị ở đây...");
    ui->groupBox->setEnabled(false);
}

void ChannelStatisticsWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // Khi widget thay đổi kích thước, gọi lại hàm cập nhật hiển thị tên kênh
    // và tính toán lại thống kê để văn bản vừa với kích thước mới.
    updateChannelNameDisplay();
    if(m_currentChannel.id != -1){
        onCalculateClicked();
    }
}


void ChannelStatisticsWidget::onCalculateClicked()
{
    if (!ui) {
        qCritical() << "[CRITICAL] Lỗi nghiêm trọng: Con trỏ 'ui' là null trong onCalculateClicked!";
        return;
    }
    
    if (!m_videoRepo) {
        ui->resultLabel->setText("Lỗi: video repository không hợp lệ.");
        return;
    }

    if (m_currentChannel.id == -1) {
        return;
    }

    QDate startDate = ui->startDateEdit->date();
    QDate endDate = ui->endDateEdit->date();

    int count = m_videoRepo->getUploadedVideoCount(m_currentChannel.id, startDate, endDate);
    
    // Thay đổi: Loại bỏ tên kênh khỏi chuỗi kết quả
    ui->resultLabel->setText(QString("Số video đã upload từ %1 đến %2 là: <b>%3</b>")
                             .arg(startDate.toString("dd/MM/yyyy"))
                             .arg(endDate.toString("dd/MM/yyyy"))
                             .arg(count));
    
    // Xóa tooltip vì không còn cần thiết
    ui->resultLabel->setToolTip("");
}

void ChannelStatisticsWidget::onTimeRangeChanged(int index)
{
    QDate currentDate = QDate::currentDate();
    QDate startDate, endDate;

    switch(index) {
        case 0: // Tuần này
            startDate = currentDate.addDays(-(currentDate.dayOfWeek() - 1));
            endDate = startDate.addDays(6);
            break;
        case 1: // Tháng này
            startDate = QDate(currentDate.year(), currentDate.month(), 1);
            endDate = startDate.addMonths(1).addDays(-1);
            break;
        case 2: // Năm nay
            startDate = QDate(currentDate.year(), 1, 1);
            endDate = QDate(currentDate.year(), 12, 31);
            break;
        case 3: // Tùy chỉnh
            ui->startDateEdit->setEnabled(true);
            ui->endDateEdit->setEnabled(true);
            return;
    }

    ui->startDateEdit->setDate(startDate);
    ui->endDateEdit->setDate(endDate);
    ui->startDateEdit->setEnabled(false);
    ui->endDateEdit->setEnabled(false);
}

void ChannelStatisticsWidget::updateChannelNameDisplay()
{
    // Chỉ thực hiện nếu có một kênh hợp lệ đang được chọn
    if (m_currentChannel.id != -1 && ui->channelNameLabel) {
        QFontMetrics metrics(ui->channelNameLabel->font());
        // Trừ đi một khoảng nhỏ để đảm bảo không bị tràn lề
        QString elidedName = metrics.elidedText(m_currentChannel.name, Qt::ElideRight, ui->channelNameLabel->width() - 5);
        ui->channelNameLabel->setText(elidedName);
        ui->channelNameLabel->setToolTip(m_currentChannel.name); // Hiển thị tên đầy đủ khi di chuột vào
    }
}
