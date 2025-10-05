// Vị trí: /src/dialogs/EditVideoDialog.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Dialog chi tiết để chỉnh sửa thông tin một video.

#pragma once

#include <QDialog>
#include "../models.h"

namespace Ui {
class EditVideoDialog;
}

class IChannelRepository;

class EditVideoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditVideoDialog(const Video& videoData, IChannelRepository* channelRepo, QWidget *parent = nullptr);
    ~EditVideoDialog();

    Video getVideoData() const;

private:
    void populateChannels();
    void setVideoData(const Video& videoData);

    Ui::EditVideoDialog *ui;
    IChannelRepository* m_channelRepo;
    Video m_video;
};
