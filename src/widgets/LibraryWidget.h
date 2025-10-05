// Vị trí: /src/widgets/LibraryWidget.h
// Phiên bản: 1.1 (Sửa lỗi Build)
// Mô tả: Widget container cho tab "Thư viện".

#pragma once

#include <QWidget>

namespace Ui {
class LibraryWidget;
}

// Forward declarations
class LibraryToolbarWidget;
class LibraryPaginationWidget;
class QTableView;

class LibraryWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryWidget(QWidget *parent = nullptr);
    ~LibraryWidget();

    // Getters cho các widget con
    LibraryToolbarWidget* getToolbarWidget() const;
    LibraryPaginationWidget* getPaginationWidget() const;
    QTableView* getTableView() const;

private:
    Ui::LibraryWidget *ui;
};

