// Vị trí: /src/controllers/manualuploadcontroller.h
// Phiên bản: 2.4 (Tái cấu trúc CSDL)

#pragma once

#include <QObject>

// Forward declarations
class ManualUploadWidget;
class IChannelRepository;
class IVideoRepository;
class QStandardItemModel;
class QItemSelection;

class ManualUploadController : public QObject
{
    Q_OBJECT
public:
    explicit ManualUploadController(ManualUploadWidget* widget, IChannelRepository* channelRepo, IVideoRepository* videoRepo, QObject *parent = nullptr);

public slots:
    void refreshCompletedVideos();
    void onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onCopyTitle();
    void onCopyDescription();
    void onCopyTags();
    void onOpenVideoFolder();
    void onOpenUploadPage();
    void onMarkAsUploaded();
    void onTabChanged(int index);

private:
    void saveCurrentUploadInfo();
    void clearUploadInfoFields();

    ManualUploadWidget* m_widget;
    IChannelRepository* m_channelRepo;
    IVideoRepository* m_videoRepo;
    QStandardItemModel* m_completedVideosModel;
    int m_currentlySelectedCompletedVideoId;
};
