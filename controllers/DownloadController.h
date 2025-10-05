// Vị trí: /src/controllers/downloadcontroller.h
// Phiên bản: 6.2 (Thêm Requeue)

#pragma once

#include <QObject>
#include <QList>
#include "../models.h"
#include "../ui_managers/downloaduimanager.h"

class DownloadWidget;
class IVideoRepository;
class DownloadQueueService;
class DownloadQueueModel;
class DownloadUIManager;

class DownloadController : public QObject
{
    Q_OBJECT
public:
    explicit DownloadController(DownloadWidget* widget, IVideoRepository* videoRepo, DownloadQueueService* queueService, QObject *parent = nullptr);
    ~DownloadController();

public slots:
    void onDownloadRequested(const QList<int>& videoIds);
    void onRequeueDownloadRequested(const QList<int>& videoIds); // Slot mới

private slots:
    // Slots for UI events
    void onAddClicked();
    void onSettingsClicked();
    void onGetFromLibraryClicked();
    void onItemActionRequested(int row, DownloadUIManager::ItemAction action);
    void onLogViewRequested(int row);

private:
    void setupConnections();
    void handlePlaylist(const QString& url);

    DownloadWidget* m_widget;
    IVideoRepository* m_videoRepo;
    DownloadQueueService* m_queueService;
    DownloadQueueModel* m_queueModel;
    DownloadUIManager* m_uiManager;
};
