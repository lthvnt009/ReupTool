// Vị trí: /src/ui_managers/datainputuimanager.h
// Phiên bản: 1.5 (Tối ưu hóa Signal)

#pragma once

#include <QObject>
#include <QList>
#include <QMap>
#include <QSet>
#include "../models.h"

class DataInputWidget;
class ChannelDataWidget;
class QFrame;
class QPushButton;
class QSignalMapper;
class QProgressBar;
class QLabel;


class DataInputUIManager : public QObject
{
    Q_OBJECT

public:
    explicit DataInputUIManager(DataInputWidget* widget, QObject* parent = nullptr);
    
    void createUI(const QList<Channel>& channels, const QMap<int, QMap<QDate, Video>>& dbVideoMap, const QMap<int, QMap<QDate, Video>>& unsavedState, const QSet<int>& completedChannelIds);
    QList<ChannelDataWidget*> getWidgets() const;
    ChannelDataWidget* findWidgetByVideoId(int videoId) const;
    QProgressBar* getProgressBarForChannel(int channelId) const;
    QLabel* getStatusLabelForChannel(int channelId) const;
    void updateChannelFrameState(int channelId, bool isComplete);


signals:
    void descriptionEditRequested(ChannelDataWidget* widget);
    void saveAllClicked();
    void importFromFileClicked();
    void settingsClicked();

private:
    void setupToolbarConnections();

    DataInputWidget* m_widget;
    QList<ChannelDataWidget*> m_channelDataWidgets;
    QMap<int, QFrame*> m_channelFrames;
    QMap<int, ChannelDataWidget*> m_videoWidgetMap;

    QMap<int, QProgressBar*> m_channelProgressBars;
    QMap<int, QLabel*> m_channelStatusLabels;
};

