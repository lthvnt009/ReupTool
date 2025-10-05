// Vị trí: /src/dialogs/LibrarySettingsDialog.h
// Phiên bản: 1.1 (Thêm nút Bảo trì)
// Mô tả: Dialog cho phép người dùng tùy chỉnh các cột hiển thị trong Thư viện.

#pragma once

#include <QDialog>
#include <QList>
#include <QCheckBox>

namespace Ui {
class LibrarySettingsDialog;
}

class QTableView;

class LibrarySettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LibrarySettingsDialog(QTableView* tableView, QWidget *parent = nullptr);
    ~LibrarySettingsDialog();

signals:
    void settingsChanged();
    void openMaintenanceDialogRequested(); // Tín hiệu mới

private slots:
    void applySettings();

private:
    void createCheckboxes();
    void loadSettings();
    void saveSettings();

    Ui::LibrarySettingsDialog *ui;
    QTableView* m_tableView;
    QList<QCheckBox*> m_checkboxes;
};

