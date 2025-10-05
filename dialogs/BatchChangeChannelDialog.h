// Vị trí: /src/dialogs/BatchChangeChannelDialog.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Dialog cho phép chuyển kênh hàng loạt.

#pragma once

#include <QDialog>

namespace Ui {
class BatchChangeChannelDialog;
}

class IChannelRepository;

class BatchChangeChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchChangeChannelDialog(IChannelRepository* channelRepo, QWidget *parent = nullptr);
    ~BatchChangeChannelDialog();

    int getSelectedChannelId() const;

private:
    void populateChannels();

    Ui::BatchChangeChannelDialog *ui;
    IChannelRepository* m_channelRepo;
};
