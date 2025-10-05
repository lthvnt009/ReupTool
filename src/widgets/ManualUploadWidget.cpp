// Phiên bản: 1.0 (Mới)
// Vị trí: /src/widgets/manualuploadwidget.cpp
// Mô tả: Triển khai cho ManualUploadWidget.

#include "manualuploadwidget.h"
#include "ui_manualuploadwidget.h"

ManualUploadWidget::ManualUploadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManualUploadWidget)
{
    ui->setupUi(this);
}

ManualUploadWidget::~ManualUploadWidget()
{
    delete ui;
}

Ui::ManualUploadWidget* ManualUploadWidget::getUi() const
{
    return ui;
}
