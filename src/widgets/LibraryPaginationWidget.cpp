// Vị trí: /src/widgets/LibraryPaginationWidget.cpp
// Phiên bản: 1.0 (Mới)

#include "librarypaginationwidget.h"
#include "ui_librarypaginationwidget.h"

LibraryPaginationWidget::LibraryPaginationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryPaginationWidget)
{
    ui->setupUi(this);

    connect(ui->firstButton, &QToolButton::clicked, this, &LibraryPaginationWidget::onFirstClicked);
    connect(ui->previousButton, &QToolButton::clicked, this, &LibraryPaginationWidget::onPreviousClicked);
    connect(ui->nextButton, &QToolButton::clicked, this, &LibraryPaginationWidget::onNextClicked);
    connect(ui->lastButton, &QToolButton::clicked, this, &LibraryPaginationWidget::onLastClicked);
    connect(ui->pageLineEdit, &QLineEdit::returnPressed, this, &LibraryPaginationWidget::onGoToPage);
}

LibraryPaginationWidget::~LibraryPaginationWidget()
{
    delete ui;
}

int LibraryPaginationWidget::getCurrentPage() const
{
    return m_currentPage;
}

void LibraryPaginationWidget::updateInfo(int currentPage, int totalPages, int totalItems, int itemsOnPage)
{
    m_currentPage = currentPage;
    m_totalPages = totalPages > 0 ? totalPages : 1;

    ui->pageLineEdit->setText(QString::number(m_currentPage));
    ui->totalPagesLabel->setText(QString("/ %1").arg(m_totalPages));

    int startItem = (totalItems > 0) ? (m_currentPage - 1) * 50 + 1 : 0;
    int endItem = startItem + itemsOnPage - 1;
    ui->infoLabel->setText(QString("Hiển thị %1 - %2 trên tổng số %3 bản ghi")
                           .arg(startItem)
                           .arg(endItem)
                           .arg(totalItems));

    ui->firstButton->setEnabled(m_currentPage > 1);
    ui->previousButton->setEnabled(m_currentPage > 1);
    ui->nextButton->setEnabled(m_currentPage < m_totalPages);
    ui->lastButton->setEnabled(m_currentPage < m_totalPages);
}

void LibraryPaginationWidget::onFirstClicked()
{
    if (m_currentPage > 1) {
        emit pageChanged(1);
    }
}

void LibraryPaginationWidget::onPreviousClicked()
{
    if (m_currentPage > 1) {
        emit pageChanged(m_currentPage - 1);
    }
}

void LibraryPaginationWidget::onNextClicked()
{
    if (m_currentPage < m_totalPages) {
        emit pageChanged(m_currentPage + 1);
    }
}

void LibraryPaginationWidget::onLastClicked()
{
    if (m_currentPage < m_totalPages) {
        emit pageChanged(m_totalPages);
    }
}

void LibraryPaginationWidget::onGoToPage()
{
    bool ok;
    int page = ui->pageLineEdit->text().toInt(&ok);
    if (ok && page >= 1 && page <= m_totalPages) {
        emit pageChanged(page);
    } else {
        // Khôi phục lại số trang hiện tại nếu nhập sai
        ui->pageLineEdit->setText(QString::number(m_currentPage));
    }
}
