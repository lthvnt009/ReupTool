// Vị trí: /src/widgets/downloadtoolbarwidget.cpp
// Phiên bản: 1.0 (Mới)
#include "downloadtoolbarwidget.h"
#include "ui_downloadtoolbarwidget.h"

DownloadToolbarWidget::DownloadToolbarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadToolbarWidget)
{
    ui->setupUi(this);
}

DownloadToolbarWidget::~DownloadToolbarWidget()
{
    delete ui;
}

Ui::DownloadToolbarWidget* DownloadToolbarWidget::getUi() const
{
    return ui;
}
