// Vị trí: /src/widgets/LibraryWidget.cpp
// Phiên bản: 1.1 (Sửa lỗi Build)

#include "librarywidget.h"
#include "ui_librarywidget.h"
#include "librarytoolbarwidget.h"
#include "librarypaginationwidget.h"
#include <QTableView>


LibraryWidget::LibraryWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryWidget)
{
    ui->setupUi(this);
}

LibraryWidget::~LibraryWidget()
{
    delete ui;
}

LibraryToolbarWidget* LibraryWidget::getToolbarWidget() const
{
    return ui->toolbarWidget;
}

LibraryPaginationWidget* LibraryWidget::getPaginationWidget() const
{
    return ui->paginationWidget;
}

QTableView* LibraryWidget::getTableView() const
{
    return ui->videoTableView;
}

