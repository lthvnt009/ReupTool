// Vị trí: /src/widgets/channeltoolbarwidget.cpp
// Phiên bản: 1.0 (Mới)

#include "channeltoolbarwidget.h"
#include "ui_channeltoolbarwidget.h"

ChannelToolbarWidget::ChannelToolbarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelToolbarWidget)
{
    ui->setupUi(this);
}

ChannelToolbarWidget::~ChannelToolbarWidget()
{
    delete ui;
}

Ui::ChannelToolbarWidget* ChannelToolbarWidget::getUi() const
{
    return ui;
}
