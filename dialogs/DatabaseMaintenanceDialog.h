// Vị trí: /src/dialogs/DatabaseMaintenanceDialog.h
// Phiên bản: 1.1 (Thêm chức năng tìm Trùng lặp)

#pragma once

#include <QDialog>
#include <QMap>
#include <QList>
#include "../models.h"

// Forward declarations
namespace Ui {
class DatabaseMaintenanceDialog;
}
class LibraryService;

class DatabaseMaintenanceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DatabaseMaintenanceDialog(LibraryService* service, QWidget *parent = nullptr);
    ~DatabaseMaintenanceDialog();

private slots:
    void onCleanupInfoReady(int count);
    void onCleanup();
    void onCleanupCompleted(bool success, const QString& message);
    void onFindDuplicates();
    void onDuplicatesFound(const QMap<QString, QList<Video>>& duplicates);

private:
    Ui::DatabaseMaintenanceDialog *ui;
    LibraryService* m_service;
};

