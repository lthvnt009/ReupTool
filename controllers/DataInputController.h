// Vị trí: /src/controllers/datainputcontroller.h
// Phiên bản: 7.3 (Tối ưu hóa Signal)

#pragma once

#include <QObject>
#include <QSet>
#include <QList>
#include <QMap>
#include <QStringList>
#include "../models.h"

// Forward declarations
class DataInputWidget;
class IChannelRepository;
class IVideoRepository;
class ChannelDataWidget;
class MetadataFetcher;
class DataInputDataService;
class DataInputUIManager;
class QUrlQuery;
class FileImportService;
class QProgressBar;
class QLabel;
class QTimer;

class DataInputController : public QObject
{
    Q_OBJECT
public:
    explicit DataInputController(DataInputWidget* widget, IChannelRepository* channelRepo, IVideoRepository* videoRepo, QObject *parent = nullptr);
    bool hasUnsavedData() const;

signals:
    void startDownloadRequested(const QList<int>& videoIds);

public slots:
    void reloadDataAndUI();
    void onDownloadProgressUpdated(int videoId, int percentage, const QString& status);
    void onVideosModified(const QList<int>& videoIds);

    // Slots for individual day actions
    void onSaveDayClicked(ChannelDataWidget* widget);
    void onEditDayClicked(ChannelDataWidget* widget);
    void onRefreshDayClicked(ChannelDataWidget* widget);
    void onCancelDayClicked(ChannelDataWidget* widget);
    void onUrlPasted(ChannelDataWidget* widget, const QString& url);
    void onDescriptionEditRequested(ChannelDataWidget* widget);

private slots:
    // Slots for UI events from UIManager & Widgets
    void onDaysSpinBoxChanged();
    void onSaveAllClicked();
    void onImportFromFile();
    void onSettingsClicked();

    // Slots for Service/Fetcher results
    void onMetadataReady(ChannelDataWidget* requester, const QString& title, const QString& description);
    void onFetchFailed(ChannelDataWidget* requester, const QString& errorMessage);
    
    // Internal logic slots
    void onPlaylistUrlPasted(ChannelDataWidget* requestingWidget, const QUrlQuery& urlQuery);

private:
    void fetchMetadata(ChannelDataWidget* widget);
    void fillDataFromPlaylist(ChannelDataWidget* startingWidget, const QStringList& urls);
    void fillDataFromFile(const QMap<QString, QStringList>& channelUrlMap);
    void saveData(int channelId = -1);
    void checkAndUpdateChannelState(int channelId);


    DataInputWidget* m_widget;
    IChannelRepository* m_channelRepo;
    IVideoRepository* m_videoRepo;
    
    DataInputUIManager* m_uiManager;
    DataInputDataService* m_dataService;
    MetadataFetcher* m_metadataFetcher;
    FileImportService* m_importService;
    
};

