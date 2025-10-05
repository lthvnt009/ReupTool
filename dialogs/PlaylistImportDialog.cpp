// Vị trí: /src/dialogs/playlistimportdialog.cpp
// Phiên bản: 1.7 (Sửa lỗi không đọc đường dẫn yt-dlp từ cài đặt)

#include "playlistimportdialog.h"
#include "ui_playlistimportdialog.h"
#include "../appsettings.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QSet>
#include <QSettings>

PlaylistImportDialog::PlaylistImportDialog(const QString &playlistUrl, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlaylistImportDialog),
    m_process(new QProcess(this)),
    m_playlistUrl(playlistUrl)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(true);
    ui->videoTableWidget->setVisible(false);
    ui->buttonBox->setEnabled(false);

    // Cấu hình cho TableWidget - đã được thực hiện trong file .ui
    ui->videoTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->videoTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(m_process, &QProcess::finished, this, &PlaylistImportDialog::onProcessFinished);

    // Cập nhật trạng thái ban đầu của nút
    updateInvertButtonState();

    // Sửa lỗi: Lấy đường dẫn yt-dlp từ AppSettings thay vì hardcode
    QSettings& settings = getAppSettings();
    QString ytdlpPath = settings.value("DownloadSettings/ytdlpPath", "lib/yt-dlp.exe").toString();
    
    QStringList arguments;
    arguments << "--flat-playlist" << "-J" << m_playlistUrl;

    m_process->start(ytdlpPath, arguments);
}

PlaylistImportDialog::~PlaylistImportDialog()
{
    delete ui;
}

QStringList PlaylistImportDialog::getSelectedUrls() const
{
    QStringList selectedUrls;
    for(int i = 0; i < ui->videoTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->videoTableWidget->item(i, 1); // Item ở cột Tên Video
        if (item && item->checkState() == Qt::Checked) {
            // Lấy URL từ dữ liệu của item ở cột STT
            selectedUrls.append(ui->videoTableWidget->item(i, 0)->data(Qt::UserRole).toString());
        }
    }
    return selectedUrls;
}

void PlaylistImportDialog::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->progressBar->setVisible(false);

    // Cải tiến: Hiển thị lỗi chi tiết hơn
    if (exitStatus != QProcess::NormalExit || exitCode != 0) {
        QString errorOutput = m_process->readAllStandardError();
        if (errorOutput.isEmpty()) {
            ui->titleLabel->setText("Lỗi: Không thể lấy danh sách video. Vui lòng kiểm tra lại đường dẫn yt-dlp.exe trong Cài đặt Tải về.");
        } else {
             ui->titleLabel->setText("Lỗi: " + errorOutput);
        }
        return;
    }

    QByteArray jsonData = m_process->readAllStandardOutput();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject rootObj = doc.object();

    ui->titleLabel->setText(QString("Playlist: %1").arg(rootObj["title"].toString()));

    m_videoEntries = rootObj["entries"].toArray();
    
    populateTable();

    ui->videoTableWidget->setVisible(true);
    ui->buttonBox->setEnabled(true);
}

void PlaylistImportDialog::populateTable(bool reversed, const QSet<QString>& preCheckedUrls)
{
    ui->videoTableWidget->clearContents();
    ui->videoTableWidget->setRowCount(m_videoEntries.count());

    for (int i = 0; i < m_videoEntries.count(); ++i) {
        int entryIndex = reversed ? (m_videoEntries.count() - 1 - i) : i;
        const QJsonObject& entryObj = m_videoEntries[entryIndex].toObject();

        QString title = entryObj["title"].toString();
        QString url = entryObj["url"].toString();

        // Cột STT (#)
        QTableWidgetItem* indexItem = new QTableWidgetItem(QString::number(i + 1));
        indexItem->setData(Qt::UserRole, url); // Lưu URL vào item STT
        indexItem->setTextAlignment(Qt::AlignCenter);

        // Cột Tên Video
        QTableWidgetItem* titleItem = new QTableWidgetItem(title);
        titleItem->setFlags(titleItem->flags() | Qt::ItemIsUserCheckable);

        // Khôi phục trạng thái đã chọn
        if (preCheckedUrls.contains(url)) {
            titleItem->setCheckState(Qt::Checked);
        } else {
            titleItem->setCheckState(Qt::Unchecked);
        }

        ui->videoTableWidget->setItem(i, 0, indexItem);
        ui->videoTableWidget->setItem(i, 1, titleItem);
    }
}


void PlaylistImportDialog::on_invertSelectionButton_clicked()
{
    for(int i = 0; i < ui->videoTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->videoTableWidget->item(i, 1);
        if(item) {
            item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
        }
    }
}

void PlaylistImportDialog::on_selectAllButton_clicked()
{
    for(int i = 0; i < ui->videoTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->videoTableWidget->item(i, 1);
        if(item) {
            item->setCheckState(Qt::Checked);
        }
    }
}

void PlaylistImportDialog::on_deselectAllButton_clicked()
{
    for(int i = 0; i < ui->videoTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *item = ui->videoTableWidget->item(i, 1);
        if(item) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void PlaylistImportDialog::on_invertOrderButton_clicked()
{
    // 1. Lưu lại các URL đang được chọn
    QSet<QString> checkedUrls;
    for(int i = 0; i < ui->videoTableWidget->rowCount(); ++i)
    {
        QTableWidgetItem *titleItem = ui->videoTableWidget->item(i, 1);
        if (titleItem && titleItem->checkState() == Qt::Checked) {
            QTableWidgetItem *indexItem = ui->videoTableWidget->item(i, 0);
            checkedUrls.insert(indexItem->data(Qt::UserRole).toString());
        }
    }

    // 2. Đảo ngược trạng thái
    m_isOrderReversed = !m_isOrderReversed;

    // 3. Cập nhật giao diện nút bấm
    updateInvertButtonState();

    // 4. Vẽ lại bảng, truyền vào danh sách đã chọn
    populateTable(m_isOrderReversed, checkedUrls);
}

void PlaylistImportDialog::on_indexSelectionLineEdit_textChanged(const QString &text)
{
    // Bỏ chọn tất cả trước khi xử lý
    on_deselectAllButton_clicked();
    if (text.isEmpty()) return;

    QSet<int> indicesToSelect;
    QStringList parts = text.split(',', Qt::SkipEmptyParts);

    for (const QString& part : parts) {
        QString trimmedPart = part.trimmed();
        if (trimmedPart.contains('-')) {
            QStringList rangeParts = trimmedPart.split('-');
            if (rangeParts.size() == 2) {
                bool ok1, ok2;
                int start = rangeParts[0].toInt(&ok1);
                int end = rangeParts[1].toInt(&ok2);
                if (ok1 && ok2 && start <= end) {
                    for (int i = start; i <= end; ++i) {
                        indicesToSelect.insert(i);
                    }
                }
            }
        } else {
            bool ok;
            int index = trimmedPart.toInt(&ok);
            if (ok) {
                indicesToSelect.insert(index);
            }
        }
    }

    for (int index : indicesToSelect) {
        int rowIndex = index - 1;
        if (rowIndex >= 0 && rowIndex < ui->videoTableWidget->rowCount()) {
            QTableWidgetItem *item = ui->videoTableWidget->item(rowIndex, 1);
            if(item) {
                item->setCheckState(Qt::Checked);
            }
        }
    }
}

void PlaylistImportDialog::updateInvertButtonState()
{
    if (m_isOrderReversed) {
        // Áp dụng style khi nút đang active (màu xanh dương)
        ui->invertOrderButton->setStyleSheet(
            "background-color: #3498db;"
            "color: white;"
            "border: 1px solid #2980b9;"
            "padding: 2px;"
            "border-radius: 4px;"
        );
    } else {
        // Xóa style để trở về mặc định của hệ điều hành
        ui->invertOrderButton->setStyleSheet("");
    }
}
