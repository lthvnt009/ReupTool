// Vị trí: /src/dialogs/downloadsettingsdialog.h
// Phiên bản: 1.4 (Loại bỏ logic Cập nhật)

#pragma once

#include <QDialog>
#include <QMap>

// Forward declarations
namespace Ui {
class DownloadSettingsDialog;
}
class QCheckBox;

class DownloadSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadSettingsDialog(QWidget *parent = nullptr);
    ~DownloadSettingsDialog();

private slots:
    void accept() override;
    void onBrowseYtdlpPath();
    void onBrowseFfmpegPath();
    void updateOutputTemplate();

private:
    void loadSettings();
    void saveSettings();

    Ui::DownloadSettingsDialog *ui;

    // Map để quản lý các checkbox của SponsorBlock
    QMap<QString, QCheckBox*> m_sbRemoveMap;
    QMap<QString, QCheckBox*> m_sbMarkMap;
};
