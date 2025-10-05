// Vị trí: /src/dialogs/channelsettingsdialog.h
// Phiên bản: 1.3 (Cải tiến giao diện theo yêu cầu)

#pragma once

#include <QDialog>
#include <QEvent>
#include <QList>
#include "../models.h"

namespace Ui {
class ChannelSettingsDialog;
}

class IChannelRepository;

class ChannelSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChannelSettingsDialog(IChannelRepository* channelRepo, QWidget *parent = nullptr);
    ~ChannelSettingsDialog();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onChannelSelectionChanged(int index);
    void onSaveSettings();
    void loadSettingsForChannel(int channelId);

private:
    Ui::ChannelSettingsDialog *ui;
    IChannelRepository* m_channelRepo;
    QList<Channel> m_channels;
};
