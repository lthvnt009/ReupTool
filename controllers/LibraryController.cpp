// Vị trí: /src/controllers/LibraryController.cpp
// Phiên bản: 2.5 (Tối ưu hóa Signal)

#include "librarycontroller.h"
#include "../widgets/librarytoolbarwidget.h"
#include "../widgets/librarypaginationwidget.h"
#include "../services/libraryservice.h"
#include "../repositories/ichannelrepository.h"
#include "../models/videolibrarymodel.h"
#include "../delegates/libraryitemdelegate.h"
#include "../dialogs/batchedittagsdialog.h"
#include "../dialogs/batchchangechanneldialog.h"
#include "../dialogs/librarysettingsdialog.h"
#include "../dialogs/editvideodialog.h"
#include "../dialogs/databasemaintenancedialog.h"
#include "../appsettings.h"

#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <QSettings>

LibraryController::LibraryController(
    LibraryToolbarWidget* toolbar,
    LibraryPaginationWidget* pagination,
    QTableView* tableView,
    LibraryService* service,
    IChannelRepository* channelRepo,
    QObject* parent)
    : QObject(parent)
    , m_toolbar(toolbar)
    , m_pagination(pagination)
    , m_tableView(tableView)
    , m_service(service)
    , m_channelRepo(channelRepo)
{
    m_toolbar->setChannelRepository(m_channelRepo);

    m_videoModel = new VideoLibraryModel(this);
    m_itemDelegate = new LibraryItemDelegate(this);

    QSortFilterProxyModel* proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(m_videoModel);
    m_tableView->setModel(proxyModel);
    m_tableView->setItemDelegate(m_itemDelegate);

    applyColumnSettings();
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setColumnWidth(VideoLibraryModel::Check, 30);
    m_tableView->setColumnWidth(VideoLibraryModel::Actions, 80);

    // UI -> Controller
    connect(m_toolbar, &LibraryToolbarWidget::importClicked, this, &LibraryController::onImportClicked);
    connect(m_toolbar, &LibraryToolbarWidget::exportClicked, this, &LibraryController::onExportClicked);
    connect(m_toolbar, &LibraryToolbarWidget::settingsClicked, this, &LibraryController::onSettingsClicked);
    connect(m_toolbar, &LibraryToolbarWidget::filterRequested, this, &LibraryController::onFilterRequested);
    connect(m_toolbar, &LibraryToolbarWidget::resetRequested, this, &LibraryController::refreshData);
    connect(m_toolbar, &LibraryToolbarWidget::batchEditTagsClicked, this, &LibraryController::onBatchEditTagsClicked);
    connect(m_toolbar, &LibraryToolbarWidget::batchChangeChannelClicked, this, &LibraryController::onBatchChangeChannelClicked);
    connect(m_toolbar, &LibraryToolbarWidget::batchDeleteClicked, this, &LibraryController::onBatchDeleteClicked);
    connect(m_pagination, &LibraryPaginationWidget::pageChanged, this, &LibraryController::onPageChanged);
    connect(m_itemDelegate, &LibraryItemDelegate::editClicked, this, &LibraryController::onEditClicked);
    connect(m_itemDelegate, &LibraryItemDelegate::deleteClicked, this, &LibraryController::onDeleteClicked);
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &LibraryController::onContextMenuRequested);
    connect(m_videoModel, &VideoLibraryModel::checkedCountChanged, m_toolbar, &LibraryToolbarWidget::onSelectionChanged);

    // Service -> Controller (để cập nhật view của Library)
    connect(m_service, &LibraryService::videosReady, this, &LibraryController::onVideosReady);
    connect(m_service, &LibraryService::countReady, this, &LibraryController::onCountReady);
    connect(m_service, &LibraryService::deleteCompleted, this, &LibraryController::onDeleteCompleted);
    connect(m_service, &LibraryService::restoreCompleted, this, &LibraryController::onRestoreCompleted);
    connect(m_service, &LibraryService::batchEditCompleted, this, &LibraryController::onBatchEditCompleted);
    connect(m_service, &LibraryService::taskProgress, this, &LibraryController::taskProgress);
    connect(m_service, &LibraryService::taskFinished, this, &LibraryController::onTaskFinished);
}

void LibraryController::refreshData()
{
    m_toolbar->resetFilters();
    m_currentCriteria = FilterCriteria();
    m_toolbar->setChannelRepository(m_channelRepo);
    fetchData();
}

void LibraryController::onUndoDeleteTriggered()
{
    m_service->requestRestoreLastDeleted();
}


void LibraryController::onImportClicked()
{
    QString filePath = QFileDialog::getOpenFileName(m_tableView, "Chọn file XLSX để nhập", "", "Excel Files (*.xlsx)");
    if (filePath.isEmpty()) return;

    emit taskStarted();
    m_service->requestImport(filePath);
}

void LibraryController::onExportClicked()
{
    QString filePath = QFileDialog::getSaveFileName(m_tableView, "Lưu file XLSX", "thu-vien-video.xlsx", "Excel Files (*.xlsx)");
    if (filePath.isEmpty()) return;
    
    emit taskStarted();
    m_service->requestExport(filePath, m_currentCriteria);
}


void LibraryController::onFilterRequested()
{
    m_currentCriteria = m_toolbar->getCurrentCriteria();
    m_currentCriteria.page = 1;
    fetchData();
}

void LibraryController::onPageChanged(int page)
{
    m_currentCriteria.page = page;
    fetchData();
}

void LibraryController::onVideosReady(const QList<Video>& videos)
{
    QHash<int, QString> channelMap;
    for(const auto& ch : m_channelRepo->loadAllChannels()) {
        channelMap[ch.id] = ch.name;
    }
    m_videoModel->setChannelMap(channelMap);
    m_videoModel->setVideos(videos);
}

void LibraryController::onCountReady(int totalItems, int itemsOnPage)
{
    int totalPages = (totalItems > 0) ? (totalItems + m_currentCriteria.pageSize - 1) / m_currentCriteria.pageSize : 1;
    m_pagination->updateInfo(m_currentCriteria.page, totalPages, totalItems, itemsOnPage);
}

void LibraryController::onEditClicked(const QModelIndex& index)
{
    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(m_tableView->model());
    QModelIndex sourceIndex = proxy->mapToSource(index);
    Video videoToEdit = m_videoModel->getVideo(sourceIndex.row());

    if (videoToEdit.id == -1) return;

    EditVideoDialog dialog(videoToEdit, m_channelRepo, m_tableView);
    if (dialog.exec() == QDialog::Accepted) {
        Video updatedVideo = dialog.getVideoData();
        m_service->requestUpdateVideo(updatedVideo);
    }
}


void LibraryController::onDeleteClicked(const QModelIndex& index)
{
    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(m_tableView->model());
    QModelIndex sourceIndex = proxy->mapToSource(index);
    int videoId = m_videoModel->data(m_videoModel->index(sourceIndex.row(), VideoLibraryModel::ID)).toInt();
    QString title = m_videoModel->data(m_videoModel->index(sourceIndex.row(), VideoLibraryModel::NewTitle)).toString();

    QMessageBox msgBox(m_tableView);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(QString("Bạn có chắc chắn muốn xóa video '%1'?").arg(title));
    msgBox.setInformativeText("Bạn có thể hoàn tác hành động này trong vài giây.");
    msgBox.setStandardButtons(QMessageBox::Cancel);
    QPushButton* deleteDbButton = msgBox.addButton("Chỉ xóa bản ghi", QMessageBox::DestructiveRole);
    QPushButton* deleteAllButton = msgBox.addButton("Xóa bản ghi và file", QMessageBox::DestructiveRole);
    
    msgBox.exec();

    QAbstractButton* clickedButton = msgBox.clickedButton();
    if (clickedButton == deleteDbButton) {
        m_service->requestDelete({videoId}, false);
    } else if (clickedButton == deleteAllButton) {
        m_service->requestDelete({videoId}, true);
    }
}

void LibraryController::onContextMenuRequested(const QPoint& pos)
{
    QModelIndex index = m_tableView->indexAt(pos);
    if (!index.isValid()) return;

    QSortFilterProxyModel* proxy = qobject_cast<QSortFilterProxyModel*>(m_tableView->model());
    QModelIndex sourceIndex = proxy->mapToSource(index);
    
    QList<int> selectedIds;
    if (m_videoModel->getCheckedVideoIds().count() > 1 && m_videoModel->isChecked(sourceIndex.row())) {
        selectedIds = m_videoModel->getCheckedVideoIds();
    } else {
        selectedIds.append(m_videoModel->getVideo(sourceIndex.row()).id);
    }

    if (selectedIds.isEmpty()) return;

    QMenu contextMenu(m_tableView);
    QAction* editAction = contextMenu.addAction("Sửa thông tin...");
    QAction* deleteAction = contextMenu.addAction("Xóa video...");
    contextMenu.addSeparator();
    QAction* copyUrlAction = contextMenu.addAction("Sao chép Link Video");
    QAction* copyTitleAction = contextMenu.addAction("Sao chép Tiêu đề");
    contextMenu.addSeparator();
    QAction* requeueAction = contextMenu.addAction("Thêm lại vào hàng đợi tải");

    editAction->setEnabled(selectedIds.count() == 1);

    QAction* selectedAction = contextMenu.exec(m_tableView->viewport()->mapToGlobal(pos));

    if (selectedAction == editAction) {
        onEditClicked(index);
    } else if (selectedAction == deleteAction) {
        if (selectedIds.count() > 1) {
            onBatchDeleteClicked();
        } else {
            onDeleteClicked(index);
        }
    } else if (selectedAction == copyUrlAction) {
        QString url = m_videoModel->data(sourceIndex.sibling(sourceIndex.row(), VideoLibraryModel::Link)).toString();
        QApplication::clipboard()->setText(url);
    } else if (selectedAction == copyTitleAction) {
        QString title = m_videoModel->data(sourceIndex.sibling(sourceIndex.row(), VideoLibraryModel::NewTitle)).toString();
        QApplication::clipboard()->setText(title);
    } else if (selectedAction == requeueAction) {
        emit requeueDownloadRequested(selectedIds);
    }
}

void LibraryController::onBatchEditTagsClicked()
{
    QList<int> ids = m_videoModel->getCheckedVideoIds();
    if (ids.isEmpty()) return;

    BatchEditTagsDialog dialog(m_tableView);
    if (dialog.exec() == QDialog::Accepted) {
        emit taskStarted();
        m_service->requestBatchEditTags(ids, dialog.getAction(), dialog.getTag1(), dialog.getTag2());
    }
}

void LibraryController::onBatchChangeChannelClicked()
{
    QList<int> ids = m_videoModel->getCheckedVideoIds();
    if (ids.isEmpty()) return;

    BatchChangeChannelDialog dialog(m_channelRepo, m_tableView);
    if (dialog.exec() == QDialog::Accepted) {
        int newChannelId = dialog.getSelectedChannelId();
        if (newChannelId <= 0) return;
        emit taskStarted();
        m_service->requestBatchChangeChannel(ids, newChannelId);
    }
}


void LibraryController::onBatchDeleteClicked()
{
    QList<int> ids = m_videoModel->getCheckedVideoIds();
    if (ids.isEmpty()) return;
    
    QMessageBox msgBox(m_tableView);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(QString("Bạn có chắc chắn muốn xóa %1 video đã chọn?").arg(ids.count()));
    msgBox.setInformativeText("Bạn có thể hoàn tác hành động này trong vài giây.");
    msgBox.setStandardButtons(QMessageBox::Cancel);
    QPushButton* deleteDbButton = msgBox.addButton("Chỉ xóa bản ghi", QMessageBox::DestructiveRole);
    QPushButton* deleteAllButton = msgBox.addButton("Xóa bản ghi và file", QMessageBox::DestructiveRole);
    
    msgBox.exec();

    QAbstractButton* clickedButton = msgBox.clickedButton();
    if (clickedButton == deleteDbButton) {
        m_service->requestDelete(ids, false);
    } else if (clickedButton == deleteAllButton) {
        m_service->requestDelete(ids, true);
    }
}

void LibraryController::onSettingsClicked()
{
    LibrarySettingsDialog dialog(m_tableView, m_tableView);
    connect(&dialog, &LibrarySettingsDialog::settingsChanged, this, &LibraryController::applyColumnSettings);
    connect(&dialog, &LibrarySettingsDialog::openMaintenanceDialogRequested, this, &LibraryController::onOpenMaintenanceDialog);
    dialog.exec();
}

void LibraryController::onOpenMaintenanceDialog()
{
    DatabaseMaintenanceDialog dialog(m_service, m_tableView);
    dialog.exec();
}

void LibraryController::onDeleteCompleted(bool success, const QString& message, const QList<int>& deletedIds)
{
    if (success) {
        emit showUndoAction(message);
        fetchData();
    } else {
        QMessageBox::critical(m_tableView, "Lỗi Xóa", message);
    }
}

void LibraryController::onRestoreCompleted(bool success, const QString& message)
{
    if (success) {
        emit taskFinished(message, 4000);
        fetchData();
    } else {
        QMessageBox::warning(m_tableView, "Lỗi Hoàn tác", message);
    }
}


void LibraryController::onBatchEditCompleted(bool success, const QString& message)
{
    emit taskFinished(message, 4000);
    if (success) {
        fetchData();
    } else {
        QMessageBox::warning(m_tableView, "Lỗi", message);
    }
}


void LibraryController::onTaskFinished(bool success, const QString& title, const QString& message)
{
    emit taskFinished(message, 4000);
    if (success) {
        QMessageBox::information(m_tableView, title, message);
        refreshData();
    } else {
        QMessageBox::warning(m_tableView, title, message);
    }
}


void LibraryController::fetchData()
{
    m_service->requestData(m_currentCriteria);
}

void LibraryController::applyColumnSettings()
{
    QSettings& settings = getAppSettings();
    QStringList hiddenColumns = settings.value("Library/hiddenColumns").toStringList();

    auto* header = m_tableView->horizontalHeader();
    for (int i = 0; i < m_videoModel->columnCount(); ++i) {
        QString headerText = m_videoModel->headerData(i, Qt::Horizontal).toString();
        if(!headerText.isEmpty()) {
            m_tableView->setColumnHidden(i, hiddenColumns.contains(headerText));
        }
    }
    
    m_tableView->setColumnHidden(VideoLibraryModel::ID, true);
    m_tableView->setColumnHidden(VideoLibraryModel::Link, true);
}

