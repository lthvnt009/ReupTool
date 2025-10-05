// Phiên bản: 1.6 (Tinh chỉnh UX cho nút Đảo ngược)
// Vị trí: /src/dialogs/playlistimportdialog.h

#pragma once

#include <QDialog>
#include <QProcess>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>
#include <QString>

namespace Ui {
class PlaylistImportDialog;
}

class PlaylistImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlaylistImportDialog(const QString& playlistUrl, QWidget *parent = nullptr);
    ~PlaylistImportDialog();

    QStringList getSelectedUrls() const;

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void on_invertSelectionButton_clicked();
    void on_selectAllButton_clicked();
    void on_deselectAllButton_clicked();
    void on_invertOrderButton_clicked();
    void on_indexSelectionLineEdit_textChanged(const QString &text);

private:
    void populateTable(bool reversed = false, const QSet<QString>& preCheckedUrls = {});
    void updateInvertButtonState(); // Hàm mới để cập nhật UI nút

    Ui::PlaylistImportDialog *ui;
    QProcess* m_process;
    QString m_playlistUrl;
    QJsonArray m_videoEntries;
    bool m_isOrderReversed = false;
};

