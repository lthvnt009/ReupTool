// Vị trí: /src/widgets/LibraryPaginationWidget.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Module giao diện thanh phân trang cho tab Thư viện.

#pragma once

#include <QWidget>

namespace Ui {
class LibraryPaginationWidget;
}

class LibraryPaginationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryPaginationWidget(QWidget *parent = nullptr);
    ~LibraryPaginationWidget();

    int getCurrentPage() const;

public slots:
    void updateInfo(int currentPage, int totalPages, int totalItems, int itemsOnPage);

signals:
    void pageChanged(int page);

private slots:
    void onFirstClicked();
    void onPreviousClicked();
    void onNextClicked();
    void onLastClicked();
    void onGoToPage();

private:
    Ui::LibraryPaginationWidget *ui;
    int m_currentPage = 1;
    int m_totalPages = 1;
};
