// Phiên bản: 2.0 (Tái cấu trúc)
// Vị trí: /src/widgets/datainputwidget.cpp
// Mô tả: Triển khai cho DataInputWidget.

#include "datainputwidget.h"
#include "ui_datainputwidget.h"
#include "datainputtoolbarwidget.h" // Include a header for the new toolbar

DataInputWidget::DataInputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataInputWidget)
{
    ui->setupUi(this);
    m_toolbarWidget = ui->toolbarWidget; // Get a pointer to the promoted widget
}

DataInputWidget::~DataInputWidget()
{
    delete ui;
}

Ui::DataInputWidget* DataInputWidget::getUi() const
{
    return ui;
}

DataInputToolbarWidget* DataInputWidget::getToolbarWidget() const
{
    return m_toolbarWidget;
}
