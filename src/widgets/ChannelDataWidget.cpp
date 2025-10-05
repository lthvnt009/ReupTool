// Vị trí: /src/widgets/channeldatawidget.cpp
// Phiên bản: 4.6 (Cải tiến chức năng Mặc định)

#include "channeldatawidget.h"
#include "ui_channeldatawidget.h"
#include "clearablelineedit.h"
#include <QPlainTextEdit>
#include <QEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

ChannelDataWidget::ChannelDataWidget(int channelId, const QDate& date, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChannelDataWidget),
    m_channelId(channelId),
    m_date(date),
    m_descriptionTextEdit(new QPlainTextEdit(this))
{
    ui->setupUi(this);
    m_descriptionTextEdit->hide();

    ui->dateLabel->setText(m_date.toString("dd/MM/yyyy"));
    ui->descriptionLineEdit->installEventFilter(this);

    // Kết nối các tín hiệu/slot nội bộ và phát ra ngoài
    connect(ui->descriptionLineEdit, &QLineEdit::textChanged, this, [this](const QString& text){
        if (m_descriptionTextEdit->toPlainText() != text) {
            m_descriptionTextEdit->setPlainText(text);
        }
    });

    connect(ui->urlLineEdit, &QLineEdit::textChanged, this, &ChannelDataWidget::onUrlTextChanged);
    connect(ui->saveButton, &QPushButton::clicked, this, &ChannelDataWidget::onSaveButtonClicked);
    connect(ui->refreshButton, &QPushButton::clicked, this, [this]{ emit refreshClicked(this); });
    connect(ui->cancelButton, &QPushButton::clicked, this, [this]{ emit cancelClicked(this); });


    setDayStatus(false);
    setSavedState(false); // Trạng thái ban đầu
}

ChannelDataWidget::~ChannelDataWidget()
{
    delete ui;
}

Video ChannelDataWidget::getVideoData()
{
    Video data;
    data.id = m_videoId;
    data.channelId = m_channelId;
    data.videoDate = m_date;
    data.videoUrl = ui->urlLineEdit->text();
    data.newTitle = ui->titleLineEdit->text();
    data.newDescription = m_descriptionTextEdit->toPlainText();
    data.newPlaylist = ui->playlistLineEdit->text();
    data.newCategory = ui->categoryLineEdit->text();

    QStringList tags, subTags;
    for(auto* edit : m_tagEdits) {
        if (!edit->text().isEmpty()) tags.append(edit->text());
    }
    for(auto* edit : m_subTagEdits) {
        if (!edit->text().isEmpty()) subTags.append(edit->text());
    }

    data.newTags = tags.join(", ");
    data.newSubTags = subTags.join(", ");

    return data;
}

bool ChannelDataWidget::hasUrl() const
{
    return !ui->urlLineEdit->text().isEmpty();
}

bool ChannelDataWidget::isSaved() const
{
    return m_isSaved;
}

void ChannelDataWidget::clearAllData()
{
    // Tạm thời khóa tín hiệu để tránh kích hoạt onUrlTextChanged
    ui->urlLineEdit->blockSignals(true);
    ui->urlLineEdit->clear();
    ui->urlLineEdit->blockSignals(false);

    ui->titleLineEdit->clear();
    ui->descriptionLineEdit->clear();
    m_descriptionTextEdit->clear();
    ui->playlistLineEdit->clear();
    ui->categoryLineEdit->clear();
    for(auto* edit : m_tagEdits) edit->clear();
    for(auto* edit : m_subTagEdits) edit->clear();

    setDayStatus(false);
}

// Cải tiến: Áp dụng các giá trị mặc định được truyền vào
void ChannelDataWidget::applyDefaultValues(const QString& category, const QString& playlist, const QStringList& tags, const QStringList& subTags)
{
     setCategory(category);
     setPlaylist(playlist);
     addTags(tags, subTags);
}

void ChannelDataWidget::setVideoData(const Video& data)
{
    m_videoId = data.id;
    setUrl(data.videoUrl);
    ui->titleLineEdit->setText(data.newTitle);
    setDescription(data.newDescription);
    ui->playlistLineEdit->setText(data.newPlaylist);
    ui->categoryLineEdit->setText(data.newCategory);

    QStringList tags = data.newTags.split(", ", Qt::SkipEmptyParts);
    QStringList subTags = data.newSubTags.split(", ", Qt::SkipEmptyParts);

    qDeleteAll(m_tagEdits);
    m_tagEdits.clear();
    QLayoutItem* item;
    while ((item = ui->tagsLayout->takeAt(0)) != nullptr) { delete item->widget(); delete item; }

    qDeleteAll(m_subTagEdits);
    m_subTagEdits.clear();
    while ((item = ui->subTagsLayout->takeAt(0)) != nullptr) { delete item->widget(); delete item; }

    addTags(tags, subTags);

    for (int i = 0; i < qMin(m_tagEdits.size(), tags.size()); ++i) {
        m_tagEdits[i]->setText(tags[i]);
    }
    for (int i = 0; i < qMin(m_subTagEdits.size(), subTags.size()); ++i) {
        m_subTagEdits[i]->setText(subTags[i]);
    }

    setDayStatus(true);
    setSavedState(true);
}

void ChannelDataWidget::setVideoId(int id)
{
    m_videoId = id;
}


void ChannelDataWidget::setUrl(const QString& url)
{
    // Sửa lỗi đệ quy: khóa tín hiệu trước khi set text
    ui->urlLineEdit->blockSignals(true);
    ui->urlLineEdit->setText(url);
    ui->urlLineEdit->blockSignals(false);
}

void ChannelDataWidget::setDescription(const QString& description)
{
    m_descriptionTextEdit->setPlainText(description);
    ui->descriptionLineEdit->setText(description);
    ui->descriptionLineEdit->setCursorPosition(0);
}

void ChannelDataWidget::setPlaylist(const QString& playlist)
{
    ui->playlistLineEdit->setText(playlist);
}

void ChannelDataWidget::setCategory(const QString& category)
{
    ui->categoryLineEdit->setText(category);
}

void ChannelDataWidget::addTags(const QStringList& tags, const QStringList& subTags)
{
    qDeleteAll(m_tagEdits);
    m_tagEdits.clear();
    QLayoutItem* item;
    while ((item = ui->tagsLayout->takeAt(0)) != nullptr) { if(item->widget()) delete item->widget(); delete item; }

    qDeleteAll(m_subTagEdits);
    m_subTagEdits.clear();
    while ((item = ui->subTagsLayout->takeAt(0)) != nullptr) { if(item->widget()) delete item->widget(); delete item; }

    for (const QString& tag : tags) {
        ClearableLineEdit* lineEdit = new ClearableLineEdit(this);
        lineEdit->setText(tag);
        ui->tagsLayout->addWidget(lineEdit);
        m_tagEdits.append(lineEdit);
    }
    for (const QString& subTag : subTags) {
        ClearableLineEdit* lineEdit = new ClearableLineEdit(this);
        lineEdit->setText(subTag);
        ui->subTagsLayout->addWidget(lineEdit);
        m_subTagEdits.append(lineEdit);
    }
}

void ChannelDataWidget::setDuplicateStatus(bool isDuplicate, const QString& message)
{
    if (isDuplicate) {
        this->setStyleSheet("ChannelDataWidget { background-color: #fff3cd; }");
        this->setToolTip(message);
    } else {
        this->setStyleSheet("");
        this->setToolTip("");
    }
}

void ChannelDataWidget::setMetadataStatus(const QString& status)
{
    ui->titleLineEdit->setText(status);
}

void ChannelDataWidget::setMetadata(const QString& title, const QString& description)
{
    ui->titleLineEdit->setText(title);
    setDescription(description);
}

void ChannelDataWidget::setSavedState(bool isSaved)
{
    m_isSaved = isSaved;
    
    // Tạo stylesheet để làm mờ
    const QString disabledStyle = "color: gray;";

    // Bật/tắt các ô nhập liệu
    ui->urlLineEdit->setEnabled(!isSaved);
    ui->titleLineEdit->setEnabled(!isSaved);
    ui->descriptionLineEdit->setEnabled(!isSaved);
    ui->playlistLineEdit->setEnabled(!isSaved);
    ui->categoryLineEdit->setEnabled(!isSaved);
    for(auto* widget : m_tagEdits) widget->setEnabled(!isSaved);
    for(auto* widget : m_subTagEdits) widget->setEnabled(!isSaved);

    // Bật/tắt các nút và làm mờ các label
    ui->refreshButton->setEnabled(!isSaved);
    ui->cancelButton->setEnabled(!isSaved);
    ui->dateLabel->setStyleSheet(isSaved ? disabledStyle : "");
    ui->label_tags->setStyleSheet(isSaved ? disabledStyle : "");
    ui->label_subtags->setStyleSheet(isSaved ? disabledStyle : "");

    // Đổi tên nút Lưu/Sửa
    ui->saveButton->setText(isSaved ? "Sửa" : "Lưu");
}

void ChannelDataWidget::setDayStatus(bool hasData)
{
    if (hasData) {
        ui->dayStatusLabel->setText("Có sẵn");
        ui->dayStatusLabel->setStyleSheet("color: green;");
    } else {
        ui->dayStatusLabel->setText("Cần tìm");
        ui->dayStatusLabel->setStyleSheet("color: orange;");
    }
}


void ChannelDataWidget::onUrlTextChanged(const QString &text)
{
    setDayStatus(!text.isEmpty());
    if (text.startsWith("http")) {
        emit urlPasted(this, text);
    }
}

void ChannelDataWidget::onSaveButtonClicked()
{
    if (m_isSaved) {
        emit editClicked(this);
    } else {
        emit saveClicked(this);
    }
}


bool ChannelDataWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->descriptionLineEdit && event->type() == QEvent::MouseButtonDblClick) {
        if (!m_isSaved) {
            emit descriptionEditRequested(this);
        }
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
