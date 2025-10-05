// Vị trí: /src/widgets/channelmanagerwidget.cpp
// Phiên bản: 2.5 (Thêm StyleSheet cho Splitter)

#include "channelmanagerwidget.h"
#include "ui_channelmanagerwidget.h"

#include "channeltoolbarwidget.h"
#include "channellistwidget.h"
#include "channelstatisticswidget.h"
#include "../repositories/ichannelrepository.h"
#include "../repositories/ivideorepository.h"
#include <QSplitter>

ChannelManagerWidget::ChannelManagerWidget(IChannelRepository* channelRepo, IVideoRepository* videoRepo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelManagerWidget)
{
    ui->setupUi(this);

    m_statisticsWidget = new ChannelStatisticsWidget(channelRepo, videoRepo, this);

    int index = ui->splitter->indexOf(ui->statisticsPlaceholder);
    ui->splitter->replaceWidget(index, m_statisticsWidget);
    delete ui->statisticsPlaceholder;

    // Cải tiến: Thêm StyleSheet để căn giữa đường phân cách
    ui->splitter->setStyleSheet(
        "QSplitter::handle:horizontal {"
        "    width: 1px;"
        "    background-color: #d0d0d0;"
        "    margin: 0px 4px;" // Tạo khoảng trống 4px mỗi bên, tổng khu vực tương tác là 9px
        "}"
    );

    // Không set kích thước cứng ở đây nữa, sẽ để cho MainWindow khôi phục
}

ChannelManagerWidget::~ChannelManagerWidget()
{
    delete ui;
}

ChannelToolbarWidget* ChannelManagerWidget::getToolbarWidget() const
{
    return ui->toolbarWidget;
}

ChannelListWidget* ChannelManagerWidget::getListWidget() const
{
    return ui->listWidget;
}

ChannelStatisticsWidget* ChannelManagerWidget::getStatisticsWidget() const
{
    return m_statisticsWidget;
}

QSplitter* ChannelManagerWidget::getSplitter() const
{
    return ui->splitter;
}
