// Vị trí: /src/widgets/downloadcontrolbarwidget.cpp
// Phiên bản: 1.0 (Mới)
#include "downloadcontrolbarwidget.h"
#include "ui_downloadcontrolbarwidget.h"

DownloadControlBarWidget::DownloadControlBarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DownloadControlBarWidget)
{
    ui->setupUi(this);
    connect(ui->addButton, &QPushButton::clicked, this, &DownloadControlBarWidget::addClicked);
    connect(ui->startButton, &QPushButton::clicked, this, &DownloadControlBarWidget::startClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &DownloadControlBarWidget::stopClicked);
}

DownloadControlBarWidget::~DownloadControlBarWidget()
{
    delete ui;
}

Ui::DownloadControlBarWidget* DownloadControlBarWidget::getUi() const
{
    return ui;
}
