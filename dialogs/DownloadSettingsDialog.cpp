// Vị trí: /src/dialogs/downloadsettingsdialog.cpp
// Phiên bản: 1.6 (Loại bỏ logic Cập nhật)

#include "downloadsettingsdialog.h"
#include "ui_downloadsettingsdialog.h"
#include "../appsettings.h"
#include <QSettings>
#include <QFileDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QMessageBox>

DownloadSettingsDialog::DownloadSettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DownloadSettingsDialog)
{
    ui->setupUi(this);

    // Khởi tạo map cho SponsorBlock Remove
    m_sbRemoveMap["sponsor"] = ui->sb_remove_sponsor;
    m_sbRemoveMap["selfpromo"] = ui->sb_remove_selfpromo;
    m_sbRemoveMap["interaction"] = ui->sb_remove_interaction;
    m_sbRemoveMap["intro"] = ui->sb_remove_intro;
    m_sbRemoveMap["outro"] = ui->sb_remove_outro;
    m_sbRemoveMap["preview"] = ui->sb_remove_preview;
    m_sbRemoveMap["music_offtopic"] = ui->sb_remove_music_offtopic;
    m_sbRemoveMap["filler"] = ui->sb_remove_filler;
    
    // Khởi tạo map cho SponsorBlock Mark
    m_sbMarkMap["sponsor"] = ui->sb_mark_sponsor;
    m_sbMarkMap["selfpromo"] = ui->sb_mark_selfpromo;
    m_sbMarkMap["interaction"] = ui->sb_mark_interaction;
    m_sbMarkMap["intro"] = ui->sb_mark_intro;
    m_sbMarkMap["outro"] = ui->sb_mark_outro;
    m_sbMarkMap["preview"] = ui->sb_mark_preview;
    m_sbMarkMap["music_offtopic"] = ui->sb_mark_music_offtopic;
    m_sbMarkMap["poi_highlight"] = ui->sb_mark_poi_highlight;

    ui->templateAddComboBox->addItems({"", "Tên kênh", "Ngày tải", "Số thứ tự"});
    loadSettings();

    // Kết nối các tín hiệu/slot
    connect(ui->browseYtdlpPathButton, &QToolButton::clicked, this, &DownloadSettingsDialog::onBrowseYtdlpPath);
    connect(ui->browseFfmpegPathButton, &QToolButton::clicked, this, &DownloadSettingsDialog::onBrowseFfmpegPath);
    connect(ui->templateAddComboBox, &QComboBox::currentTextChanged, this, &DownloadSettingsDialog::updateOutputTemplate);
    connect(ui->addAsPrefixRadioButton, &QRadioButton::toggled, this, &DownloadSettingsDialog::updateOutputTemplate);
    connect(ui->addAsSuffixRadioButton, &QRadioButton::toggled, this, &DownloadSettingsDialog::updateOutputTemplate);
    connect(ui->addPlaylistIndexToNameCheckBox, &QCheckBox::toggled, this, &DownloadSettingsDialog::updateOutputTemplate);
}

DownloadSettingsDialog::~DownloadSettingsDialog()
{
    delete ui;
}

void DownloadSettingsDialog::loadSettings()
{
    QSettings& settings = getAppSettings();
    settings.beginGroup("DownloadSettings");

    ui->singleVideoTemplateLineEdit->setText(settings.value("videoTemplate", "%(title)s.%(ext)s").toString());
    ui->playlistVideoTemplateLineEdit->setText(settings.value("playlistTemplate", "%(playlist_index)s - %(title)s.%(ext)s").toString());
    ui->addPlaylistIndexToNameCheckBox->setChecked(settings.value("addPlaylistIndex", true).toBool());
    ui->createPlaylistFolderCheckBox->setChecked(settings.value("createPlaylistFolder", true).toBool());
    ui->ytdlpPathLineEdit->setText(settings.value("ytdlpPath", "lib/yt-dlp.exe").toString());
    ui->ffmpegPathLineEdit->setText(settings.value("ffmpegPath", "lib/ffmpeg/bin").toString());
    ui->concurrentDownloadsSpinBox->setValue(settings.value("concurrentDownloads", 1).toInt());
    ui->autoStartNextCheckBox->setChecked(settings.value("autoStartNext", true).toBool());
    ui->autoAddFromClipboardCheckBox->setChecked(settings.value("autoAddFromClipboard", false).toBool());
    ui->autoStartOnPauseCheckBox->setChecked(settings.value("autoStartOnPause", true).toBool());
    
    QStringList sbRemove = settings.value("sponsorBlockRemove").toStringList();
    for(auto it = m_sbRemoveMap.begin(); it != m_sbRemoveMap.end(); ++it) {
        it.value()->setChecked(sbRemove.contains(it.key()));
    }
    QStringList sbMark = settings.value("sponsorBlockMark").toStringList();
    for(auto it = m_sbMarkMap.begin(); it != m_sbMarkMap.end(); ++it) {
        it.value()->setChecked(sbMark.contains(it.key()));
    }

    settings.endGroup();
}

void DownloadSettingsDialog::saveSettings()
{
    QSettings& settings = getAppSettings();
    settings.beginGroup("DownloadSettings");

    settings.setValue("videoTemplate", ui->singleVideoTemplateLineEdit->text());
    settings.setValue("playlistTemplate", ui->playlistVideoTemplateLineEdit->text());
    settings.setValue("addPlaylistIndex", ui->addPlaylistIndexToNameCheckBox->isChecked());
    settings.setValue("createPlaylistFolder", ui->createPlaylistFolderCheckBox->isChecked());
    settings.setValue("ytdlpPath", ui->ytdlpPathLineEdit->text());
    settings.setValue("ffmpegPath", ui->ffmpegPathLineEdit->text());
    settings.setValue("concurrentDownloads", ui->concurrentDownloadsSpinBox->value());
    settings.setValue("autoStartNext", ui->autoStartNextCheckBox->isChecked());
    settings.setValue("autoAddFromClipboard", ui->autoAddFromClipboardCheckBox->isChecked());
    settings.setValue("autoStartOnPause", ui->autoStartOnPauseCheckBox->isChecked());

    QStringList sbRemove;
    for(auto it = m_sbRemoveMap.begin(); it != m_sbRemoveMap.end(); ++it) {
        if(it.value()->isChecked()) sbRemove.append(it.key());
    }
    settings.setValue("sponsorBlockRemove", sbRemove);

    QStringList sbMark;
     for(auto it = m_sbMarkMap.begin(); it != m_sbMarkMap.end(); ++it) {
        if(it.value()->isChecked()) sbMark.append(it.key());
    }
    settings.setValue("sponsorBlockMark", sbMark);

    settings.endGroup();
}

void DownloadSettingsDialog::onBrowseYtdlpPath()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Chọn file yt-dlp.exe", "", "Executable (*.exe)");
    if (!filePath.isEmpty()) {
        ui->ytdlpPathLineEdit->setText(filePath);
    }
}

void DownloadSettingsDialog::onBrowseFfmpegPath()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "Chọn thư mục chứa ffmpeg");
    if (!dirPath.isEmpty()) {
        ui->ffmpegPathLineEdit->setText(dirPath);
    }
}

void DownloadSettingsDialog::updateOutputTemplate()
{
    QString toAdd = ui->templateAddComboBox->currentText();
    if(toAdd.isEmpty()) return;

    QString tag;
    if(toAdd == "Tên kênh") tag = "%(channel)s";
    else if(toAdd == "Ngày tải") tag = "%(upload_date)s";
    else if(toAdd == "Số thứ tự") tag = "%(autonumber)s";
    
    if(ui->addAsPrefixRadioButton->isChecked()){
        ui->singleVideoTemplateLineEdit->setText(tag + " - " + ui->singleVideoTemplateLineEdit->text());
    } else {
        ui->singleVideoTemplateLineEdit->setText(ui->singleVideoTemplateLineEdit->text() + " - " + tag);
    }

    if(ui->addPlaylistIndexToNameCheckBox->isChecked()){
        ui->playlistVideoTemplateLineEdit->setText("%(playlist_index)s - " + ui->playlistVideoTemplateLineEdit->text().remove("%(playlist_index)s - "));
    } else {
         ui->playlistVideoTemplateLineEdit->setText(ui->playlistVideoTemplateLineEdit->text().remove("%(playlist_index)s - "));
    }
    ui->templateAddComboBox->setCurrentIndex(0);
}

void DownloadSettingsDialog::accept()
{
    saveSettings();
    QDialog::accept();
}
