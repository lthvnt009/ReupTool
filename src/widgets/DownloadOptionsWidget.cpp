// Vị trí: /src/widgets/downloadoptionswidget.cpp
// Phiên bản: 1.0 (Mới)
#include "downloadoptionswidget.h"
#include "ui_downloadoptionswidget.h"

DownloadOptionsWidget::DownloadOptionsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadOptionsWidget)
{
    ui->setupUi(this);
}

DownloadOptionsWidget::~DownloadOptionsWidget()
{
    delete ui;
}

Ui::DownloadOptionsWidget* DownloadOptionsWidget::getUi() const
{
    return ui;
}
