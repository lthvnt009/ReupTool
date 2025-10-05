// Vị trí: /src/dialogs/EditVideoDialog.cpp
// Phiên bản: 1.0 (Mới)

#include "editvideodialog.h"
#include "ui_editvideodialog.h"
#include "../repositories/ichannelrepository.h"

EditVideoDialog::EditVideoDialog(const Video& videoData, IChannelRepository* channelRepo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditVideoDialog),
    m_channelRepo(channelRepo),
    m_video(videoData)
{
    ui->setupUi(this);
    populateChannels();
    setVideoData(videoData);
}

EditVideoDialog::~EditVideoDialog()
{
    delete ui;
}

Video EditVideoDialog::getVideoData() const
{
    Video updatedVideo = m_video;
    updatedVideo.channelId = ui->channelComboBox->currentData().toInt();
    updatedVideo.videoUrl = ui->videoUrlLineEdit->text();
    updatedVideo.videoDate = ui->videoDateEdit->date();
    updatedVideo.newTitle = ui->newTitleLineEdit->text();
    updatedVideo.newDescription = ui->descriptionTextEdit->toPlainText();
    updatedVideo.newTags = ui->tagsLineEdit->text();
    updatedVideo.newSubTags = ui->subTagsLineEdit->text();
    updatedVideo.newPlaylist = ui->playlistLineEdit->text();
    updatedVideo.newCategory = ui->categoryLineEdit->text();
    updatedVideo.status = ui->statusComboBox->currentData().toString();
    return updatedVideo;
}

void EditVideoDialog::populateChannels()
{
    if (!m_channelRepo) return;
    ui->channelComboBox->clear();
    QList<Channel> channels = m_channelRepo->loadAllChannels();
    for (const auto& ch : channels) {
        ui->channelComboBox->addItem(ch.name, ch.id);
    }
}

void EditVideoDialog::setVideoData(const Video& videoData)
{
    int channelIndex = ui->channelComboBox->findData(videoData.channelId);
    if (channelIndex != -1) {
        ui->channelComboBox->setCurrentIndex(channelIndex);
    }

    ui->videoUrlLineEdit->setText(videoData.videoUrl);
    ui->videoDateEdit->setDate(videoData.videoDate);
    ui->newTitleLineEdit->setText(videoData.newTitle);
    ui->descriptionTextEdit->setPlainText(videoData.newDescription);
    ui->tagsLineEdit->setText(videoData.newTags);
    ui->subTagsLineEdit->setText(videoData.newSubTags);
    ui->playlistLineEdit->setText(videoData.newPlaylist);
    ui->categoryLineEdit->setText(videoData.newCategory);

    ui->statusComboBox->addItem("Đang chờ", "Pending");
    ui->statusComboBox->addItem("Đang tải", "Downloading");
    ui->statusComboBox->addItem("Lỗi tải", "Failed");
    ui->statusComboBox->addItem("Đã tải xong", "Completed");
    ui->statusComboBox->addItem("Đã Upload", "Uploaded");

    int statusIndex = ui->statusComboBox->findData(videoData.status);
    if (statusIndex != -1) {
        ui->statusComboBox->setCurrentIndex(statusIndex);
    }
}
