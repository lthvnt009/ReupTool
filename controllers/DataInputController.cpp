// Vị trí: /src/controllers/datainputcontroller.cpp
// Phiên bản: 7.5 (Cải tiến chức năng Mặc định)

#include "datainputcontroller.h"
#include "../widgets/datainputwidget.h"
#include "../widgets/datainputtoolbarwidget.h"
#include "ui_datainputtoolbarwidget.h"
#include "../services/metadatafetcher.h"
#include "../services/datainputdataservice.h"
#include "../services/fileimportservice.h"
#include "../ui_managers/datainputuimanager.h"
#include "../dialogs/texteditordialog.h"
#include "../dialogs/playlistimportdialog.h"
#include "../dialogs/datainputsettingsdialog.h"
#include "../widgets/channeldatawidget.h"
#include "../repositories/ichannelrepository.h"
#include "../repositories/ivideorepository.h"
#include "../appsettings.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QUrl>
#include <QUrlQuery>
#include <QSettings>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QSet>

DataInputController::DataInputController(DataInputWidget* widget, IChannelRepository* channelRepo, IVideoRepository* videoRepo, QObject *parent)
    : QObject(parent)
    , m_widget(widget)
    , m_channelRepo(channelRepo)
    , m_videoRepo(videoRepo)
    , m_uiManager(new DataInputUIManager(widget, this))
    , m_dataService(new DataInputDataService(videoRepo, this))
    , m_metadataFetcher(new MetadataFetcher(this))
    , m_importService(new FileImportService(this))
{
    connect(m_uiManager, &DataInputUIManager::saveAllClicked, this, &DataInputController::onSaveAllClicked);
    connect(m_uiManager, &DataInputUIManager::importFromFileClicked, this, &DataInputController::onImportFromFile);
    connect(m_uiManager, &DataInputUIManager::settingsClicked, this, &DataInputController::onSettingsClicked);
    connect(m_uiManager, &DataInputUIManager::descriptionEditRequested, this, &DataInputController::onDescriptionEditRequested);

    connect(m_widget->getToolbarWidget()->getUi()->daysSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &DataInputController::onDaysSpinBoxChanged);
    
    connect(m_metadataFetcher, &MetadataFetcher::metadataReady, this, &DataInputController::onMetadataReady);
    connect(m_metadataFetcher, &MetadataFetcher::fetchFailed, this, &DataInputController::onFetchFailed);
    connect(m_importService, &FileImportService::errorOccurred, this, [this](const QString& msg){
        QMessageBox::critical(m_widget, "Lỗi Nhập File", msg);
    });
    
    reloadDataAndUI();
}

bool DataInputController::hasUnsavedData() const
{
    for (auto* widget : m_uiManager->getWidgets())
    {
        if(widget->hasUrl() && !widget->isSaved())
        {
            return true;
        }
    }
    return false;
}

void DataInputController::reloadDataAndUI()
{
    QMap<int, QMap<QDate, Video>> unsavedState;
    for(auto* widget : m_uiManager->getWidgets()) {
        if(widget->hasUrl() && !widget->isSaved()) {
            unsavedState[widget->getChannelId()][widget->getDate()] = widget->getVideoData();
        }
    }

    int days = m_widget->getToolbarWidget()->getUi()->daysSpinBox->value();
    QDate startDate = QDate::currentDate();

    QList<Channel> channels = m_channelRepo->loadVisibleChannels();
    QList<int> channelIds;
    for(const auto& ch : channels) channelIds.append(ch.id);

    QMap<int, QMap<QDate, Video>> videoMap;
    if(!channelIds.isEmpty()){
        QList<Video> videosInView = m_dataService->getVideosForDates(channelIds, startDate, startDate.addDays(days-1));
        for(const auto& video : videosInView) {
            videoMap[video.channelId][video.videoDate] = video;
        }
    }
    
    QSet<int> completedChannelIds;
    QList<Channel> completedChannelsList;
    QList<Channel> incompleteChannelsList;

    for(const auto& ch : channels) {
        bool isComplete = true;
        for(int i = 0; i < days; ++i) {
            QDate currentDate = startDate.addDays(i);
            if (!videoMap.contains(ch.id) || !videoMap.value(ch.id).contains(currentDate)) {
                 isComplete = false;
                 break;
            }
        }
        
        if (isComplete) {
            completedChannelIds.insert(ch.id);
            completedChannelsList.append(ch);
        } else {
            incompleteChannelsList.append(ch);
        }
    }

    QList<Channel> displayOrderChannels;
    QSettings& settings = getAppSettings();
    bool moveDownOnSave = settings.value("DataInput/moveDownOnSave", false).toBool();

    if (moveDownOnSave) {
        displayOrderChannels = incompleteChannelsList + completedChannelsList;
    } else {
        displayOrderChannels = channels;
    }

    m_uiManager->createUI(displayOrderChannels, videoMap, unsavedState, completedChannelIds);
}

void DataInputController::onDownloadProgressUpdated(int videoId, int percentage, const QString& status)
{
    ChannelDataWidget* widget = m_uiManager->findWidgetByVideoId(videoId);
    if(widget) {
        int channelId = widget->getChannelId();
        QProgressBar* progressBar = m_uiManager->getProgressBarForChannel(channelId);
        QLabel* statusLabel = m_uiManager->getStatusLabelForChannel(channelId);

        if (progressBar && statusLabel) {
            progressBar->show();
            statusLabel->setText(status);
            progressBar->setValue(percentage);

            if (percentage == 100 || status == "Lỗi" || status == "Hoàn thành") {
                QTimer::singleShot(3000, this, [progressBar, statusLabel](){
                    progressBar->hide();
                    statusLabel->setText("Sẵn sàng");
                });
            }
        }
    }
}

void DataInputController::onVideosModified(const QList<int>& videoIds)
{
    QSet<int> affectedChannelIds;
    for (int videoId : videoIds) {
        ChannelDataWidget* widget = m_uiManager->findWidgetByVideoId(videoId);
        if (widget) {
            Video updatedVideo = m_videoRepo->getVideoById(videoId);
            if (updatedVideo.id != -1) { // Video still exists (restored or edited)
                widget->setVideoData(updatedVideo);
            } else { // Video was deleted
                widget->clearAllData();
                
                // Tải lại và áp dụng cài đặt mặc định
                QSettings& settings = getAppSettings();
                settings.beginGroup(QString("Channel/%1").arg(widget->getChannelId()));
                const QString defaultCategory = settings.value("DefaultCategory", "").toString();
                const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
                const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
                const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
                settings.endGroup();
                widget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
            }
            affectedChannelIds.insert(widget->getChannelId());
        }
    }

    for (int channelId : affectedChannelIds) {
        checkAndUpdateChannelState(channelId);
    }
}


void DataInputController::onDaysSpinBoxChanged()
{
    m_metadataFetcher->clearQueueAndStop();
    reloadDataAndUI();
}

void DataInputController::onSaveAllClicked() { saveData(); }

void DataInputController::onSaveDayClicked(ChannelDataWidget* widget)
{
    if (!widget) return;

    Video videoToSave = widget->getVideoData();

    if (videoToSave.newTitle.isEmpty() || videoToSave.newTitle == "Đang lấy thông tin...") {
        QMessageBox::warning(m_widget, "Thiếu thông tin", "Không thể lưu video khi chưa có Tiêu đề hợp lệ.");
        return;
    }

    QList<int> savedIds = m_dataService->saveDataAndGetIds({videoToSave});

    if(!savedIds.isEmpty()){
        widget->setVideoId(savedIds.first());
        widget->setSavedState(true);
        checkAndUpdateChannelState(widget->getChannelId());

        if (m_widget->getToolbarWidget()->getUi()->downloadNowCheckBox->isChecked()) {
             emit startDownloadRequested(savedIds);
        }
    } else {
        QMessageBox::critical(m_widget, "Lỗi", "Không thể lưu dữ liệu vào cơ sở dữ liệu.");
    }
}

void DataInputController::onEditDayClicked(ChannelDataWidget* widget)
{
    if (widget) {
        widget->setSavedState(false);
        checkAndUpdateChannelState(widget->getChannelId());
    }
}

void DataInputController::onRefreshDayClicked(ChannelDataWidget* widget)
{
    if (widget) {
        widget->clearAllData();
        
        // Cải tiến: Tải lại và áp dụng cài đặt mặc định
        QSettings& settings = getAppSettings();
        settings.beginGroup(QString("Channel/%1").arg(widget->getChannelId()));
        const QString defaultCategory = settings.value("DefaultCategory", "").toString();
        const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
        const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
        const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
        settings.endGroup();

        widget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
    }
}

void DataInputController::onCancelDayClicked(ChannelDataWidget* widget)
{
    if (!widget) return;
    int videoId = widget->getVideoData().id;
    if (videoId > 0) {
        Video originalVideo = m_videoRepo->getVideoById(videoId);
        widget->setVideoData(originalVideo);
    } else {
        widget->clearAllData();
        
        // Cải tiến: Tải lại và áp dụng cài đặt mặc định khi hủy
        QSettings& settings = getAppSettings();
        settings.beginGroup(QString("Channel/%1").arg(widget->getChannelId()));
        const QString defaultCategory = settings.value("DefaultCategory", "").toString();
        const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
        const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
        const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
        settings.endGroup();

        widget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
    }
}

void DataInputController::saveData(int channelId)
{
    QList<Video> videosToSave;
    for (auto* widget : m_uiManager->getWidgets()) {
        if ((channelId == -1 || widget->getChannelId() == channelId) && widget->hasUrl() && !widget->isSaved()) {
            Video videoToSave = widget->getVideoData();
             if (videoToSave.newTitle.isEmpty() || videoToSave.newTitle == "Đang lấy thông tin...") {
                continue;
            }
            videosToSave.append(videoToSave);
        }
    }

    if (!videosToSave.isEmpty()) {
        QList<int> savedVideoIds = m_dataService->saveDataAndGetIds(videosToSave);
        
        if (m_widget->getToolbarWidget()->getUi()->downloadNowCheckBox->isChecked()) {
             emit startDownloadRequested(savedVideoIds);
        }
    }
    
    reloadDataAndUI();
}

void DataInputController::onUrlPasted(ChannelDataWidget* widget, const QString& url)
{
    // Cải tiến: Xóa dữ liệu cũ và áp dụng mặc định trước khi điền link mới
    widget->clearAllData();
    QSettings& settings = getAppSettings();
    settings.beginGroup(QString("Channel/%1").arg(widget->getChannelId()));
    const QString defaultCategory = settings.value("DefaultCategory", "").toString();
    const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
    const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
    const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
    settings.endGroup();
    widget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);

    widget->setUrl(url);

    QUrl qUrl(url);
    QUrlQuery query(qUrl);

    if (query.hasQueryItem("list") || qUrl.path().contains("playlist")) {
        auto reply = QMessageBox::question(m_widget, "Phát hiện Playlist", "Link này thuộc về một playlist. Bạn muốn:\n\n- Nhập cả Playlist?\n- Chỉ lấy video từ link này?", "Nhập cả Playlist", "Chỉ lấy video");
        if (reply == 0) {
            onPlaylistUrlPasted(widget, query);
        }
        else {
            query.removeQueryItem("list");
            query.removeQueryItem("index");
            qUrl.setQuery(query);
            widget->setUrl(qUrl.toString());
            fetchMetadata(widget);
        }
    } else {
        fetchMetadata(widget);
    }
}

void DataInputController::onDescriptionEditRequested(ChannelDataWidget* widget)
{
    TextEditorDialog dialog(widget->getVideoData().newDescription, m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        widget->setDescription(dialog.getText());
    }
}

void DataInputController::onImportFromFile()
{
    QString filePath = QFileDialog::getOpenFileName(m_widget, "Chọn file để nhập", "", "Tất cả file được hỗ trợ (*.txt *.csv *.xlsx);;Text Files (*.txt);;CSV/Excel Files (*.csv *.xlsx)");
    if (filePath.isEmpty()) {
        return;
    }

    QMap<QString, QStringList> channelUrlMap = m_importService->importLinksForDataInput(filePath);

    if (channelUrlMap.isEmpty()) {
        QMessageBox::information(m_widget, "Thông báo", "Không tìm thấy dữ liệu hợp lệ nào trong file.");
        return;
    }

    fillDataFromFile(channelUrlMap);
}

void DataInputController::onSettingsClicked()
{
    DataInputSettingsDialog dialog(m_widget);
    dialog.exec();
}

void DataInputController::onMetadataReady(ChannelDataWidget* requester, const QString& title, const QString& description)
{
    if (m_uiManager->getWidgets().contains(requester)) {
        requester->setMetadata(title, description);
    }
}

void DataInputController::onFetchFailed(ChannelDataWidget* requester, const QString& errorMessage)
{
    if (m_uiManager->getWidgets().contains(requester)) {
        requester->setMetadataStatus(errorMessage);
    }
}

void DataInputController::onPlaylistUrlPasted(ChannelDataWidget* requestingWidget, const QUrlQuery& urlQuery)
{
    QString playlistId = urlQuery.queryItemValue("list");
    if(playlistId.isEmpty() && requestingWidget->getVideoData().videoUrl.contains("/playlist?list=")){
        playlistId = requestingWidget->getVideoData().videoUrl.split("/playlist?list=").last();
    }
    
    QString playlistUrl = "https://www.youtube.com/playlist?list=" + playlistId;
    PlaylistImportDialog dialog(playlistUrl, m_widget);
    if (dialog.exec() == QDialog::Accepted) {
        fillDataFromPlaylist(requestingWidget, dialog.getSelectedUrls());
    } else {
        requestingWidget->clearAllData();
        // Cải tiến: Áp dụng lại mặc định nếu người dùng hủy
        QSettings& settings = getAppSettings();
        settings.beginGroup(QString("Channel/%1").arg(requestingWidget->getChannelId()));
        const QString defaultCategory = settings.value("DefaultCategory", "").toString();
        const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
        const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
        const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
        settings.endGroup();
        requestingWidget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
    }
}

void DataInputController::fetchMetadata(ChannelDataWidget* widget)
{
    if (!widget || !widget->hasUrl()) return;
    if (m_dataService->videoUrlExists(widget->getVideoData().videoUrl)) {
        widget->setDuplicateStatus(true, "Video đã tồn tại trong CSDL.");
        return;
    }
    widget->setDuplicateStatus(false, "");
    widget->setMetadataStatus("Đang lấy thông tin...");
    m_metadataFetcher->fetch(widget);
}

void DataInputController::fillDataFromPlaylist(ChannelDataWidget* startingWidget, const QStringList& urls)
{
    if (urls.isEmpty()) {
         return;
    }
    int channelId = startingWidget->getChannelId();
    int urlIndex = 0;

    // Cải tiến: Xóa dữ liệu và áp dụng mặc định cho widget bắt đầu
    startingWidget->clearAllData();
    QSettings& settings = getAppSettings();
    settings.beginGroup(QString("Channel/%1").arg(channelId));
    const QString defaultCategory = settings.value("DefaultCategory", "").toString();
    const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
    const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
    const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
    settings.endGroup();
    startingWidget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
    
    startingWidget->setUrl(urls.at(urlIndex));
    fetchMetadata(startingWidget);
    urlIndex++;

    int widgetStartIndex = m_uiManager->getWidgets().indexOf(startingWidget);
    if(widgetStartIndex != -1) {
        for(int i = widgetStartIndex + 1; i < m_uiManager->getWidgets().size() && urlIndex < urls.size(); ++i) {
            ChannelDataWidget* currentWidget = m_uiManager->getWidgets().at(i);
            if(currentWidget->getChannelId() == channelId && !currentWidget->hasUrl()) {
                currentWidget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
                currentWidget->setUrl(urls.at(urlIndex));
                fetchMetadata(currentWidget);
                urlIndex++;
            }
        }
    }

    QMessageBox::information(m_widget, "Hoàn tất", QString("Đã điền %1 link từ playlist.").arg(urlIndex));
}

void DataInputController::fillDataFromFile(const QMap<QString, QStringList>& channelUrlMap)
{
    QMap<QString, int> channelNameToIdMap;
    for(const auto& ch : m_channelRepo->loadAllChannels()) {
        channelNameToIdMap[ch.name] = ch.id;
    }

    int totalFilled = 0;
    for (auto it = channelUrlMap.constBegin(); it != channelUrlMap.constEnd(); ++it) {
        const QString& channelName = it.key();
        const QStringList& urls = it.value();

        if (!channelNameToIdMap.contains(channelName)) {
            continue;
        }
        int channelId = channelNameToIdMap[channelName];
        
        // Cải tiến: Lấy cài đặt mặc định một lần cho mỗi kênh
        QSettings& settings = getAppSettings();
        settings.beginGroup(QString("Channel/%1").arg(channelId));
        const QString defaultCategory = settings.value("DefaultCategory", "").toString();
        const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
        const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
        const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
        settings.endGroup();
        
        int urlIndex = 0;
        for (auto* widget : m_uiManager->getWidgets()) {
            if (urlIndex >= urls.size()) break; 

            if (widget->getChannelId() == channelId && !widget->hasUrl()) {
                widget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
                widget->setUrl(urls.at(urlIndex));
                fetchMetadata(widget);
                urlIndex++;
                totalFilled++;
            }
        }
    }

    QString message = QString("Đã điền thông tin cho %1 video.").arg(totalFilled);
    QMessageBox::information(m_widget, "Hoàn tất", message);
}

void DataInputController::checkAndUpdateChannelState(int channelId)
{
    bool allSaved = true;
    for(auto* widget : m_uiManager->getWidgets()) {
        if (widget->getChannelId() == channelId) {
            if (!widget->isSaved()) {
                allSaved = false;
                break;
            }
        }
    }
    m_uiManager->updateChannelFrameState(channelId, allSaved);

    if (allSaved) {
        QSettings& settings = getAppSettings();
        bool moveDownOnSave = settings.value("DataInput/moveDownOnSave", false).toBool();
        if (moveDownOnSave) {
            reloadDataAndUI();
        }
    }
}
