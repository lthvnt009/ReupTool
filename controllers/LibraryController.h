// Vị trí: /src/controllers/LibraryController.h
// Phiên bản: 2.5 (Tối ưu hóa Signal)

#pragma once

#include <QObject>
#include "../models/FilterCriteria.h"
#include "../models.h"
#include <QList>

// Forward declarations
class LibraryToolbarWidget;
class LibraryPaginationWidget;
class QTableView;
class LibraryService;
class IChannelRepository;
class VideoLibraryModel;
class LibraryItemDelegate;
class QModelIndex;
class QPoint;

class LibraryController : public QObject
{
    Q_OBJECT
public:
    explicit LibraryController(
        LibraryToolbarWidget* toolbar,
        LibraryPaginationWidget* pagination,
        QTableView* tableView,
        LibraryService* service,
        IChannelRepository* channelRepo,
        QObject* parent = nullptr
    );

public slots:
    void refreshData();
    void onUndoDeleteTriggered();

private slots:
    // Slots for UI events
    void onImportClicked();
    void onExportClicked();
    void onFilterRequested();
    void onPageChanged(int page);
    void onEditClicked(const QModelIndex& index);
    void onDeleteClicked(const QModelIndex& index);
    void onContextMenuRequested(const QPoint& pos);
    void onBatchEditTagsClicked();
    void onBatchChangeChannelClicked();
    void onBatchDeleteClicked();
    void onSettingsClicked();
    void onOpenMaintenanceDialog();


    // Slots for Service results
    void onVideosReady(const QList<Video>& videos);
    void onCountReady(int totalItems, int itemsOnPage);
    void onDeleteCompleted(bool success, const QString& message, const QList<int>& deletedIds);
    void onRestoreCompleted(bool success, const QString& message);
    void onBatchEditCompleted(bool success, const QString& message);
    void onTaskFinished(bool success, const QString& title, const QString& message);


private:
    void fetchData();
    void applyColumnSettings();

    // UI Components
    LibraryToolbarWidget* m_toolbar;
    LibraryPaginationWidget* m_pagination;
    QTableView* m_tableView;

    // Business Logic & Data
    LibraryService* m_service;
    IChannelRepository* m_channelRepo;
    VideoLibraryModel* m_videoModel;
    LibraryItemDelegate* m_itemDelegate;

    // State
    FilterCriteria m_currentCriteria;

signals:
    void requeueDownloadRequested(const QList<int>& videoIds);

    // Tín hiệu cho MainWindow status bar
    void taskStarted();
    void taskProgress(int value);
    void taskFinished(const QString& message, int timeout = 4000);
    void showUndoAction(const QString& message);
};

