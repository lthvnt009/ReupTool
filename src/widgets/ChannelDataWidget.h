// Vị trí: /src/widgets/channeldatawidget.h
// Phiên bản: 4.6 (Cải tiến chức năng Mặc định)

#pragma once

#include <QWidget>
#include <QDate>
#include <QList>
#include "../models.h"

namespace Ui {
class ChannelDataWidget;
}

class QPlainTextEdit;
class ClearableLineEdit;
class QPushButton;

class ChannelDataWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ChannelDataWidget(int channelId, const QDate& date, QWidget *parent = nullptr);
    ~ChannelDataWidget();

    Video getVideoData();
    QDate getDate() const { return m_date; }
    int getChannelId() const { return m_channelId; }
    bool hasUrl() const;
    bool isSaved() const;

    void clearAllData();
    // Thay đổi: Hàm giờ đây nhận các giá trị mặc định để áp dụng
    void applyDefaultValues(const QString& category, const QString& playlist, const QStringList& tags, const QStringList& subTags);
    void setVideoData(const Video& data);
    void setVideoId(int id);
    void setUrl(const QString& url);
    void setDescription(const QString& description);
    void setPlaylist(const QString& playlist);
    void setCategory(const QString& category);
    void addTags(const QStringList& tags, const QStringList& subTags);
    void setDuplicateStatus(bool isDuplicate, const QString& message);
    void setMetadataStatus(const QString& status);
    void setMetadata(const QString& title, const QString& description);
    void setSavedState(bool isSaved);
    void setDayStatus(bool hasData);


signals:
    void urlPasted(ChannelDataWidget* widget, const QString& url);
    void descriptionEditRequested(ChannelDataWidget* widget);
    void saveClicked(ChannelDataWidget* widget);
    void editClicked(ChannelDataWidget* widget);
    void refreshClicked(ChannelDataWidget* widget);
    void cancelClicked(ChannelDataWidget* widget);


private slots:
    void onUrlTextChanged(const QString &text);
    void onSaveButtonClicked();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::ChannelDataWidget *ui;
    int m_channelId;
    QDate m_date;
    QList<ClearableLineEdit*> m_tagEdits;
    QList<ClearableLineEdit*> m_subTagEdits;
    QPlainTextEdit* m_descriptionTextEdit;
    int m_videoId = 0;
    bool m_isSaved = false;
};
