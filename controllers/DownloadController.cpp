// Vị trí: /src/controllers/downloadcontroller.cpp
// Phiên bản: 6.3 (Thêm Requeue)

#include "downloadcontroller.h"
#include "../widgets/downloadwidget.h"
#include "../dialogs/downloadsettingsdialog.h"
#include "../dialogs/playlistimportdialog.h"
#include "../services/downloadqueueservice.h"
#include "../models/downloadqueuemodel.h"
#include "../repositories/ivideorepository.h"
#include "../ui_managers/downloaduimanager.h"

#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QUrl>
#include <QInputDialog>
#include <QDesktopServices>
#include <QSettings>
#include <QFileInfo>

DownloadController::DownloadController(DownloadWidget* widget, IVideoRepository* videoRepo, DownloadQueueService* queueService, QObject *parent)
    : QObject(parent)
    , m_widget(widget)
    , m_videoRepo(videoRepo)
    , m_queueService(queueService)
    , m_queueModel(new DownloadQueueModel(this))
{
    m_queueModel->setQueue(m_queueService->getQueue());
    m_uiManager = new DownloadUIManager(m_widget, m_queueModel, this);
    
    setupConnections();
}

DownloadController::~DownloadController()
{
}

void DownloadController::setupConnections()
{
    // Kết nối tín hiệu từ Service/Model đến UI Manager
    connect(m_queueService, &DownloadQueueService::queueChanged, m_uiManager, &DownloadUIManager::onQueueChanged);
    connect(m_queueService, &DownloadQueueService::itemChanged, m_uiManager, &DownloadUIManager::onItemChanged);
    connect(m_queueService, &DownloadQueueService::logMessageReady, m_uiManager, &DownloadUIManager::displayLog);

    // Kết nối tín hiệu từ UI Manager đến Controller
    connect(m_uiManager, &DownloadUIManager::addFromClipboardClicked, this, &DownloadController::onAddClicked);
    connect(m_uiManager, &DownloadUIManager::startAllClicked, m_queueService, &DownloadQueueService::startAll);
    connect(m_uiManager, &DownloadUIManager::stopAllClicked, m_queueService, &DownloadQueueService::stopAll);
    connect(m_uiManager, &DownloadUIManager::settingsClicked, this, &DownloadController::onSettingsClicked);
    connect(m_uiManager, &DownloadUIManager::getFromLibraryClicked, this, &DownloadController::onGetFromLibraryClicked);
    connect(m_uiManager, &DownloadUIManager::itemActionRequested, this, &DownloadController::onItemActionRequested);
    connect(m_uiManager, &DownloadUIManager::logViewRequested, m_queueService, &DownloadQueueService::viewLog);
}

void DownloadController::onDownloadRequested(const QList<int>& videoIds)
{
    if (videoIds.isEmpty()) return;
    
    QList<Video> videosToDownload;
    QList<Video> allPending = m_videoRepo->loadPendingVideos(); 
    for(int id : videoIds) {
        for(const auto& video : allPending) {
            if(video.id == id) {
                videosToDownload.append(video);
                break;
            }
        }
    }

    if (!videosToDownload.isEmpty()) {
        m_queueService->addLegacyDownloads(videosToDownload, m_uiManager->getCurrentOptionsFromUi());
    }
}

void DownloadController::onRequeueDownloadRequested(const QList<int>& videoIds)
{
    if (videoIds.isEmpty() || !m_videoRepo) return;

    QList<Video> videosToDownload;
    for (int id : videoIds) {
        Video v = m_videoRepo->getVideoById(id);
        if (v.id != -1) {
            videosToDownload.append(v);
        }
    }

    if (!videosToDownload.isEmpty()) {
        m_queueService->addLegacyDownloads(videosToDownload, m_uiManager->getCurrentOptionsFromUi());
        QMessageBox::information(m_widget, "Thông báo", QString("Đã thêm %1 video vào hàng đợi tải.").arg(videosToDownload.count()));
    }
}


void DownloadController::onAddClicked()
{
    const QString text = QApplication::clipboard()->text();
    if (text.isEmpty()) return;

    DownloadOptions currentOpts = m_uiManager->getCurrentOptionsFromUi();
    if(currentOpts.savePath.isEmpty()){
        QMessageBox::warning(m_widget, "Thiếu thông tin", "Vui lòng chọn thư mục lưu file.");
        return;
    }

    for (const QString& urlStr : text.split('\n', Qt::SkipEmptyParts)) {
        QString trimmedUrl = urlStr.trimmed();
        if (!trimmedUrl.startsWith("http")) continue;
        
        QUrl url(trimmedUrl);
        if(url.query().contains("list=") && !currentOpts.downloadPlaylist) {
            handlePlaylist(trimmedUrl);
        } else {
            m_queueService->addItem(trimmedUrl, currentOpts);
        }
    }
}


void DownloadController::onSettingsClicked()
{
    DownloadSettingsDialog dialog(m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        m_uiManager->applySettings();
        QSettings settings;
        int concurrentDownloads = settings.value("DownloadSettings/concurrentDownloads", 1).toInt();
        m_queueService->setMaxConcurrentDownloads(concurrentDownloads);
    }
}

void DownloadController::onGetFromLibraryClicked()
{
    QList<Video> pendingVideos = m_videoRepo->loadPendingVideos();
    if (pendingVideos.isEmpty()) return;
    m_queueService->addLegacyDownloads(pendingVideos, m_uiManager->getCurrentOptionsFromUi());
}

void DownloadController::onItemActionRequested(int row, DownloadUIManager::ItemAction action)
{
    const DownloadItem& item = m_queueService->getItem(row);

    switch(action) {
        case DownloadUIManager::ItemAction::TogglePause:
            m_queueService->togglePause(row);
            break;
        case DownloadUIManager::ItemAction::Rename: {
            QString currentName = QFileInfo(item.title).baseName();
            bool ok;
            QString newName = QInputDialog::getText(m_widget, "Đặt Tên File", "Nhập tên file mới (không bao gồm đuôi file):", QLineEdit::Normal, currentName, &ok);
            if (ok && !newName.isEmpty()) m_queueService->renameItem(row, newName);
            break;
        }
        case DownloadUIManager::ItemAction::Refresh:
            m_queueService->refreshItem(row);
            break;
        case DownloadUIManager::ItemAction::Delete:
            m_queueService->removeItem(row);
            break;
        case DownloadUIManager::ItemAction::Skip:
            m_queueService->skipItem(row);
            break;
        case DownloadUIManager::ItemAction::OpenFolder:
            QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(item.localFilePath).absolutePath()));
            break;
        case DownloadUIManager::ItemAction::CopyUrl:
            QApplication::clipboard()->setText(item.url);
            break;
        case DownloadUIManager::ItemAction::CopyError:
            QApplication::clipboard()->setText(item.errorMessage);
            break;
    }
}

void DownloadController::onLogViewRequested(int row)
{
    m_queueService->viewLog(row);
}

void DownloadController::handlePlaylist(const QString& url)
{
    PlaylistImportDialog dialog(url, m_widget);
    if(dialog.exec() == QDialog::Accepted) {
        DownloadOptions currentOpts = m_uiManager->getCurrentOptionsFromUi();
        for(const QString& videoUrl : dialog.getSelectedUrls()) {
             m_queueService->addItem(videoUrl, currentOpts);
        }
    }
}
