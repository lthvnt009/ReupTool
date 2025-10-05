// Phiên bản: 1.1 (Cải tiến UX Tải)
// Vị trí: /src/widgets/datainputtoolbarwidget.cpp

#include "datainputtoolbarwidget.h"
#include "ui_datainputtoolbarwidget.h"

DataInputToolbarWidget::DataInputToolbarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataInputToolbarWidget)
{
    ui->setupUi(this);
    // Sắp xếp lại thứ tự và đặt mặc định là "Hôm nay"
    ui->downloadScopeComboBox->addItems({"Hôm nay", "Ngày mai", "Tất cả"});
    ui->downloadScopeComboBox->setCurrentIndex(0);
    ui->downloadScopeComboBox->setVisible(false); // Ẩn ban đầu
}

DataInputToolbarWidget::~DataInputToolbarWidget()
{
    delete ui;
}

Ui::DataInputToolbarWidget* DataInputToolbarWidget::getUi() const
{
    return ui;
}

