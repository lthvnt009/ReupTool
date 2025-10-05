// Vị trí: /src/dialogs/DatabaseMaintenanceDialog.cpp
// Phiên bản: 1.1 (Thêm chức năng tìm Trùng lặp)

#include "databasemaintenancedialog.h"
#include "ui_databasemaintenancedialog.h"
#include "../services/libraryservice.h"
#include <QMessageBox>
#include <QTreeWidgetItem>

DatabaseMaintenanceDialog::DatabaseMaintenanceDialog(LibraryService* service, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseMaintenanceDialog),
    m_service(service)
{
    ui->setupUi(this);

    // Kết nối các tín hiệu/slot
    connect(ui->cleanupButton, &QPushButton::clicked, this, &DatabaseMaintenanceDialog::onCleanup);
    connect(ui->findDuplicatesButton, &QPushButton::clicked, this, &DatabaseMaintenanceDialog::onFindDuplicates);
    connect(m_service, &LibraryService::cleanupInfoReady, this, &DatabaseMaintenanceDialog::onCleanupInfoReady);
    connect(m_service, &LibraryService::cleanupCompleted, this, &DatabaseMaintenanceDialog::onCleanupCompleted);
    connect(m_service, &LibraryService::duplicatesFound, this, &DatabaseMaintenanceDialog::onDuplicatesFound);

    // Thiết lập cho TreeWidget
    ui->duplicatesTreeWidget->setColumnWidth(0, 400);
    ui->duplicatesTreeWidget->setColumnHidden(1, true); // Ẩn cột ID

    // Yêu cầu thông tin ban đầu
    m_service->requestCleanupInfo();
}

DatabaseMaintenanceDialog::~DatabaseMaintenanceDialog()
{
    delete ui;
}

void DatabaseMaintenanceDialog::onCleanupInfoReady(int count)
{
    if (count > 0) {
        ui->cleanupInfoLabel->setText(QString("Tìm thấy %1 video đã bị xóa và có thể dọn dẹp để giải phóng dung lượng.").arg(count));
        ui->cleanupButton->setEnabled(true);
    } else {
        ui->cleanupInfoLabel->setText("Không có video nào cần dọn dẹp. Cơ sở dữ liệu của bạn đã sạch sẽ.");
        ui->cleanupButton->setEnabled(false);
    }
}

void DatabaseMaintenanceDialog::onCleanup()
{
    auto reply = QMessageBox::question(this, "Xác nhận Dọn dẹp",
        "Hành động này sẽ xóa vĩnh viễn tất cả các video đã bị xóa khỏi cơ sở dữ liệu và không thể hoàn tác.\nBạn có chắc chắn muốn tiếp tục?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        ui->cleanupButton->setEnabled(false);
        ui->cleanupInfoLabel->setText("Đang dọn dẹp...");
        m_service->requestCleanup();
    }
}

void DatabaseMaintenanceDialog::onCleanupCompleted(bool success, const QString& message)
{
    if (success) {
        QMessageBox::information(this, "Hoàn tất", message);
        m_service->requestCleanupInfo(); // Cập nhật lại thông tin
    } else {
        QMessageBox::critical(this, "Lỗi", message);
    }
}

void DatabaseMaintenanceDialog::onFindDuplicates()
{
    ui->findDuplicatesButton->setEnabled(false);
    ui->findDuplicatesButton->setText("Đang tìm...");
    m_service->requestFindDuplicates();
}

void DatabaseMaintenanceDialog::onDuplicatesFound(const QMap<QString, QList<Video>>& duplicates)
{
    ui->findDuplicatesButton->setEnabled(true);
    ui->findDuplicatesButton->setText("Tìm kiếm Trùng lặp");
    ui->duplicatesTreeWidget->clear();

    if (duplicates.isEmpty()) {
        QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->duplicatesTreeWidget);
        rootItem->setText(0, "Không tìm thấy bản ghi nào trùng lặp.");
        return;
    }

    for (auto it = duplicates.constBegin(); it != duplicates.constEnd(); ++it) {
        QTreeWidgetItem* rootItem = new QTreeWidgetItem(ui->duplicatesTreeWidget);
        rootItem->setText(0, QString("Link: %1 (%2 bản ghi)").arg(it.key()).arg(it.value().count()));
        rootItem->setFlags(rootItem->flags() & ~Qt::ItemIsSelectable);

        for (const Video& video : it.value()) {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(rootItem);
            childItem->setText(0, QString("Tiêu đề: %1 | Trạng thái: %2").arg(video.newTitle).arg(video.status));
            childItem->setText(1, QString::number(video.id));
        }
    }
    ui->duplicatesTreeWidget->expandAll();
}

