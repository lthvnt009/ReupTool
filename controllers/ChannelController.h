// Vị trí: /src/controllers/channelcontroller.h
// Phiên bản: 4.2 (Thêm Drag & Drop)

#pragma once

#include <QObject>
#include <QList>
#include "../models.h"

// Forward declarations
class ChannelManagerWidget;
class IChannelRepository;
class ChannelModel; // Thay thế QStandardItemModel
class FileImportService;
class QStandardItem;
class QItemSelection;


class ChannelController : public QObject
{
    Q_OBJECT
public:
    explicit ChannelController(ChannelManagerWidget* widget, IChannelRepository* channelRepo, QObject* parent = nullptr);
    void loadChannelsFromDb();

public slots:
    void onAddChannel();
    void onImportChannels();
    void onEditChannel();
    void onDeleteChannel();
    void updateButtonStates();

signals:
    void channelsChanged();

private slots:
    void onMoveUp();
    void onMoveDown();
    void onSettingsClicked();
    void onChannelSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onItemChanged(QStandardItem* item);
    void onOrderChanged(); // Slot mới để xử lý sau khi kéo thả

private:
    ChannelManagerWidget* m_widget;
    IChannelRepository* m_channelRepo;
    FileImportService* m_importService;
    ChannelModel* m_channelModel; // Sử dụng model tùy chỉnh
    QList<Channel> m_channels;
};

