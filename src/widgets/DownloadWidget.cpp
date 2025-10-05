// Vị trí: /src/widgets/downloadwidget.cpp
// Phiên bản: 2.0 (Tái cấu trúc)

#include "downloadwidget.h"
#include "ui_downloadwidget.h"
#include "downloadtoolbarwidget.h"
#include "downloadcontrolbarwidget.h"
#include "downloadoptionswidget.h"

DownloadWidget::DownloadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadWidget)
{
    ui->setupUi(this);
}

DownloadWidget::~DownloadWidget()
{
    delete ui;
}

Ui::DownloadWidget* DownloadWidget::getUi() const
{
    return ui;
}

DownloadToolbarWidget* DownloadWidget::getToolbarWidget() const
{
    return ui->toolbarWidget;
}

DownloadControlBarWidget* DownloadWidget::getControlBarWidget() const
{
    return ui->controlBarWidget;
}

DownloadOptionsWidget* DownloadWidget::getOptionsWidget() const
{
    return ui->optionsWidget;
}
