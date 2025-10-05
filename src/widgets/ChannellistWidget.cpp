// Vị trí: /src/widgets/channellistwidget.cpp
// Phiên bản: 1.0 (Mới)

#include "channellistwidget.h"
#include "ui_channellistwidget.h"

ChannelListWidget::ChannelListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelListWidget)
{
    ui->setupUi(this);
}

ChannelListWidget::~ChannelListWidget()
{
    delete ui;
}

Ui::ChannelListWidget* ChannelListWidget::getUi() const
{
    return ui;
}
