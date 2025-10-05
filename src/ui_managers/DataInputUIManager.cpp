// Vị trí: /src/ui_managers/datainputuimanager.cpp
// Phiên bản: 1.7 (Cải tiến chức năng Mặc định)
#include "datainputuimanager.h"
#include "../widgets/datainputwidget.h"
#include "ui_datainputwidget.h"
#include "../widgets/datainputtoolbarwidget.h"
#include "ui_datainputtoolbarwidget.h"
#include "../widgets/channeldatawidget.h"
#include "../controllers/datainputcontroller.h"
#include "../appsettings.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSignalMapper>
#include <QProgressBar>
#include <QStyle>

DataInputUIManager::DataInputUIManager(DataInputWidget* widget, QObject* parent)
    : QObject(parent), m_widget(widget)
{
    setupToolbarConnections();
}

void DataInputUIManager::setupToolbarConnections()
{
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    connect(toolbarUi->saveAllToolButton, &QToolButton::clicked, this, &DataInputUIManager::saveAllClicked);
    connect(toolbarUi->importToolButton, &QToolButton::clicked, this, &DataInputUIManager::importFromFileClicked);
    connect(toolbarUi->settingsToolButton, &QToolButton::clicked, this, &DataInputUIManager::settingsClicked);
}

void DataInputUIManager::createUI(const QList<Channel>& channels, const QMap<int, QMap<QDate, Video>>& dbVideoMap, const QMap<int, QMap<QDate, Video>>& unsavedState, const QSet<int>& completedChannelIds)
{
    auto ui = m_widget->getUi();
    
    QWidget* oldContentWidget = ui->scrollArea->takeWidget();
    if(oldContentWidget) {
        delete oldContentWidget;
    }
    QWidget* newContentWidget = new QWidget();
    ui->scrollArea->setWidget(newContentWidget);

    m_channelDataWidgets.clear();
    m_channelFrames.clear();
    m_channelProgressBars.clear();
    m_channelStatusLabels.clear();
    m_videoWidgetMap.clear();


    QVBoxLayout* mainLayout = new QVBoxLayout(newContentWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(9, 0, 9, 9);
    
    int days = m_widget->getToolbarWidget()->getUi()->daysSpinBox->value();
    QDate startDate = QDate::currentDate();

    for(const auto& ch : channels)
    {
        QFrame* channelGroupFrame = new QFrame(newContentWidget);
        channelGroupFrame->setObjectName("channelGroupFrame");
        channelGroupFrame->setFrameShape(QFrame::StyledPanel);
        m_channelFrames.insert(ch.id, channelGroupFrame);

        channelGroupFrame->setProperty("complete", completedChannelIds.contains(ch.id));

        QVBoxLayout* channelLayout = new QVBoxLayout(channelGroupFrame);
        channelLayout->setSpacing(6);
        channelLayout->setContentsMargins(9, 9, 9, 9);

        QWidget* titleBar = new QWidget(channelGroupFrame);
        titleBar->setObjectName("titleBar");
        QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
        titleLayout->setContentsMargins(15, 2, 5, 2);

        QLabel* channelNameLabel = new QLabel(QString("<b>%1</b>").arg(ch.name), titleBar);
        channelNameLabel->setObjectName("channelNameLabel");
        
        QProgressBar* progressBar = new QProgressBar(titleBar);
        progressBar->setMaximumSize(120, 16777215);
        progressBar->setTextVisible(true);
        progressBar->hide();
        m_channelProgressBars.insert(ch.id, progressBar);

        QLabel* statusLabel = new QLabel("Sẵn sàng", titleBar);
        statusLabel->setMinimumWidth(100);
        statusLabel->setAlignment(Qt::AlignCenter);
        m_channelStatusLabels.insert(ch.id, statusLabel);


        titleLayout->addWidget(channelNameLabel);
        titleLayout->addStretch();
        titleLayout->addWidget(progressBar);
        titleLayout->addWidget(statusLabel);

        channelLayout->addWidget(titleBar);

        // Cải tiến: Tải cài đặt mặc định cho kênh này
        QSettings& settings = getAppSettings();
        settings.beginGroup(QString("Channel/%1").arg(ch.id));
        const QString defaultCategory = settings.value("DefaultCategory", "").toString();
        const QString defaultPlaylist = settings.value("DefaultPlaylist", "").toString();
        const QStringList defaultTags = settings.value("DefaultTags", "").toString().split('\n', Qt::SkipEmptyParts);
        const QStringList defaultSubTags = settings.value("DefaultSubTags", "").toString().split('\n', Qt::SkipEmptyParts);
        settings.endGroup();

        for(int i = 0; i < days; ++i)
        {
            QDate currentDate = startDate.addDays(i);
            ChannelDataWidget* widget = new ChannelDataWidget(ch.id, currentDate, channelGroupFrame);
            m_channelDataWidgets.append(widget);

            DataInputController* controller = qobject_cast<DataInputController*>(this->parent());
            if (controller) {
                 connect(widget, &ChannelDataWidget::urlPasted, controller, &DataInputController::onUrlPasted);
                 connect(widget, &ChannelDataWidget::descriptionEditRequested, controller, &DataInputController::onDescriptionEditRequested);
                 connect(widget, &ChannelDataWidget::saveClicked, controller, &DataInputController::onSaveDayClicked);
                 connect(widget, &ChannelDataWidget::editClicked, controller, &DataInputController::onEditDayClicked);
                 connect(widget, &ChannelDataWidget::refreshClicked, controller, &DataInputController::onRefreshDayClicked);
                 connect(widget, &ChannelDataWidget::cancelClicked, controller, &DataInputController::onCancelDayClicked);
            }
            
            Video videoData;
            bool hasData = false;
            if (unsavedState.contains(ch.id) && unsavedState[ch.id].contains(currentDate)) {
                 videoData = unsavedState[ch.id][currentDate];
                 widget->setVideoData(videoData);
                 widget->setSavedState(false);
                 hasData = true;
            }
            else if (dbVideoMap.contains(ch.id) && dbVideoMap[ch.id].contains(currentDate)) {
                videoData = dbVideoMap[ch.id][currentDate];
                widget->setVideoData(videoData);
                hasData = true;
            }
            
            // Cải tiến: Chỉ áp dụng mặc định nếu không có dữ liệu nào được tải
            if (!hasData) {
                widget->applyDefaultValues(defaultCategory, defaultPlaylist, defaultTags, defaultSubTags);
            }
            
            if (hasData && videoData.id > 0) {
                m_videoWidgetMap.insert(videoData.id, widget);
            }

            channelLayout->addWidget(widget);

            if(i < days - 1) {
                QFrame* line = new QFrame(channelGroupFrame);
                line->setFrameShape(QFrame::HLine);
                line->setFrameShadow(QFrame::Sunken);
                line->setObjectName("daySeparator");
                channelLayout->addWidget(line);
            }
        }
        mainLayout->addWidget(channelGroupFrame);
    }
    mainLayout->addStretch();

    newContentWidget->setStyleSheet(R"(
        QFrame#channelGroupFrame {
            border: 1px solid #c0c0c0;
            border-radius: 4px;
            background-color: none;
        }
        QFrame#channelGroupFrame[complete="true"] {
            background-color: #f0f0f0;
        }

        QWidget#titleBar {
            background-color: #3498db;
            border-top-left-radius: 3px;
            border-top-right-radius: 3px;
        }
        QFrame#channelGroupFrame[complete="true"] QWidget#titleBar {
            background-color: #d0d0d0;
        }

        QLabel#channelNameLabel {
             color: white;
             background-color: transparent;
        }
        QFrame#channelGroupFrame[complete="true"] QLabel#channelNameLabel {
            color: black;
        }

        QFrame#daySeparator {
            border: none;
            border-top: 2px solid #a0a0a0;
        }
    )");
}


QList<ChannelDataWidget*> DataInputUIManager::getWidgets() const
{
    return m_channelDataWidgets;
}

ChannelDataWidget* DataInputUIManager::findWidgetByVideoId(int videoId) const
{
    return m_videoWidgetMap.value(videoId, nullptr);
}


QProgressBar* DataInputUIManager::getProgressBarForChannel(int channelId) const
{
    return m_channelProgressBars.value(channelId, nullptr);
}

QLabel* DataInputUIManager::getStatusLabelForChannel(int channelId) const
{
    return m_channelStatusLabels.value(channelId, nullptr);
}

void DataInputUIManager::updateChannelFrameState(int channelId, bool isComplete)
{
    QFrame* frame = m_channelFrames.value(channelId, nullptr);
    if (!frame) return;

    frame->setProperty("complete", isComplete);

    frame->style()->unpolish(frame);
    frame->style()->polish(frame);
}
