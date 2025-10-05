// Phiên bản: 2.0 (Tái cấu trúc)
// Vị trí: /src/dialogs/datainputsettingsdialog.h

#pragma once

#include <QDialog>

namespace Ui {
class DataInputSettingsDialog;
}

class DataInputSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataInputSettingsDialog(QWidget *parent = nullptr);
    ~DataInputSettingsDialog();

private slots:
    void accept() override; // Ghi đè hàm accept để lưu cài đặt

private:
    void loadSettings();
    void saveSettings();

    Ui::DataInputSettingsDialog *ui;
};
