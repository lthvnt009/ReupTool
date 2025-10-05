// Vị trí: /src/controllers/ManualUploadController.cpp
// Phiên bản: 2.6 (Sửa lỗi Build)

#include "manualuploadcontroller.h"
#include "../widgets/manualuploadwidget.h"
#include "ui_manualuploadwidget.h"
#include "../repositories/ichannelrepository.h"
#include "../repositories/ivideorepository.h"

#include <QStandardItemModel>
#include <QMessageBox>
#include <QItemSelection>
#include <QStandardItem>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QHash>

ManualUploadController::ManualUploadController(ManualUploadWidget* widget, IChannelRepository* channelRepo, IVideoRepository* videoRepo, QObject *parent)
    : QObject(parent)
    , m_widget(widget)
    , m_channelRepo(channelRepo)
    , m_videoRepo(videoRepo)
    , m_completedVideosModel(new QStandardItemModel(0, 3, this))
    , m_currentlySelectedCompletedVideoId(-1)
{
    if (!m_widget || !m_widget->getUi()) {
        qCritical() << "ManualUploadWidget or its UI is null!";
        return;
    }
    auto ui = m_widget->getUi();

    m_completedVideosModel->setHorizontalHeaderLabels({"ID", "Kênh", "Tiêu Đề Gốc"});
    ui->completedVideosTableView->setModel(m_completedVideosModel);
    ui->completedVideosTableView->setColumnHidden(0, true);
    ui->completedVideosTableView->setColumnWidth(1, 150);
    ui->completedVideosTableView->horizontalHeader()->setStretchLastSection(true);

    ui->uploadGroupBox->setEnabled(false);
    
    // Connect signals
    connect(ui->completedVideosTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ManualUploadController::onSelectionChanged);
    connect(ui->btnCopyTitle, &QPushButton::clicked, this, &ManualUploadController::onCopyTitle);
    connect(ui->btnCopyDescription, &QPushButton::clicked, this, &ManualUploadController::onCopyDescription);
    connect(ui->btnCopyTags, &QPushButton::clicked, this, &ManualUploadController::onCopyTags);
    connect(ui->btnOpenVideoFolder, &QPushButton::clicked, this, &ManualUploadController::onOpenVideoFolder);
    connect(ui->btnOpenUploadPage, &QPushButton::clicked, this, &ManualUploadController::onOpenUploadPage);
    connect(ui->btnMarkAsUploaded, &QPushButton::clicked, this, &ManualUploadController::onMarkAsUploaded);
}

void ManualUploadController::refreshCompletedVideos()
{
    saveCurrentUploadInfo();
    m_completedVideosModel->removeRows(0, m_completedVideosModel->rowCount());
    if (!m_videoRepo || !m_channelRepo) return;
    
    QList<Video> videos = m_videoRepo->loadCompletedVideos();
    QList<Channel> channels = m_channelRepo->loadAllChannels();
    QHash<int, QString> channelIdToNameMap;
    for(const auto& ch : channels) {
        channelIdToNameMap[ch.id] = ch.name;
    }

    for (const auto& v : videos)
    {
        QList<QStandardItem *> row;
        auto idItem = new QStandardItem();
        idItem->setData(v.id, Qt::DisplayRole);
        row.append(idItem);
        row.append(new QStandardItem(channelIdToNameMap.value(v.channelId, "Không rõ")));
        row.append(new QStandardItem(v.originalTitle));
        m_completedVideosModel->appendRow(row);
    }
    m_widget->getUi()->completedVideosTableView->selectionModel()->clear();
    m_widget->getUi()->uploadGroupBox->setEnabled(false);
    clearUploadInfoFields();
    m_currentlySelectedCompletedVideoId = -1;
}

void ManualUploadController::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (!deselected.isEmpty() && m_currentlySelectedCompletedVideoId != -1) {
        saveCurrentUploadInfo();
    }

    auto ui = m_widget->getUi();    
    QModelIndexList indexes = selected.indexes();
    if (indexes.isEmpty()) {
        ui->uploadGroupBox->setEnabled(false);
        m_currentlySelectedCompletedVideoId = -1;
        clearUploadInfoFields();
        return;
    }
    
    int row = indexes.first().row();
    int videoId = m_completedVideosModel->item(row, 0)->data(Qt::DisplayRole).toInt();
    m_currentlySelectedCompletedVideoId = videoId;

    Video selectedVideo = m_videoRepo->getVideoById(videoId);

    if (selectedVideo.id != -1) {
        ui->uploadGroupBox->setEnabled(true);
        ui->filePathLineEdit->setText(selectedVideo.localFilePath);
        ui->newTitleLineEdit->setText(selectedVideo.newTitle.isEmpty() ? selectedVideo.originalTitle : selectedVideo.newTitle);
        ui->descriptionTextEdit->setPlainText(selectedVideo.newDescription);
        ui->tagsLineEdit->setText(selectedVideo.newTags);
        ui->categoryLineEdit->setText(selectedVideo.newCategory);
    }
}

void ManualUploadController::onCopyTitle() { QApplication::clipboard()->setText(m_widget->getUi()->newTitleLineEdit->text()); }
void ManualUploadController::onCopyDescription() { QApplication::clipboard()->setText(m_widget->getUi()->descriptionTextEdit->toPlainText()); }
void ManualUploadController::onCopyTags() { QApplication::clipboard()->setText(m_widget->getUi()->tagsLineEdit->text()); }

void ManualUploadController::onOpenVideoFolder()
{
    QFileInfo fileInfo(m_widget->getUi()->filePathLineEdit->text());
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
}

void ManualUploadController::onOpenUploadPage()
{
    if (m_currentlySelectedCompletedVideoId == -1) return;
    
    Video video = m_videoRepo->getVideoById(m_currentlySelectedCompletedVideoId);
    if(video.id != -1 && video.channelId != -1) {
        Channel ch = m_channelRepo->getChannelById(video.channelId);
        if(!ch.link.isEmpty()) {
            QDesktopServices::openUrl(QUrl(ch.link + "/upload"));
        }
    }
}

void ManualUploadController::onMarkAsUploaded()
{
    if (m_currentlySelectedCompletedVideoId == -1) {
        return;
    }

    auto reply = QMessageBox::question(m_widget, "Xác nhận", "Bạn có chắc đã upload xong video này?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        saveCurrentUploadInfo();
        m_videoRepo->markVideoAsUploaded(m_currentlySelectedCompletedVideoId);
        refreshCompletedVideos();
    }
}


void ManualUploadController::onTabChanged(int index)
{
    // This logic is now handled in MainWindow
}

void ManualUploadController::saveCurrentUploadInfo()
{
    if (m_currentlySelectedCompletedVideoId != -1) {
        auto ui = m_widget->getUi();
        Video video = m_videoRepo->getVideoById(m_currentlySelectedCompletedVideoId);
        if(video.id != -1) {
            video.newTitle = ui->newTitleLineEdit->text();
            video.newDescription = ui->descriptionTextEdit->toPlainText();
            video.newTags = ui->tagsLineEdit->text();
            video.newCategory = ui->categoryLineEdit->text();
            m_videoRepo->updateVideoUploadInfo(video); // Sửa lỗi: Gọi đúng phương thức
        }
    }
}

void ManualUploadController::clearUploadInfoFields()
{
    auto ui = m_widget->getUi();
    ui->filePathLineEdit->clear();
    ui->newTitleLineEdit->clear();
    ui->descriptionTextEdit->clear();
    ui->tagsLineEdit->clear();
    ui->categoryLineEdit->clear();
}
