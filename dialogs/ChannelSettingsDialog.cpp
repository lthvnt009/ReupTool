// Vị trí: /src/dialogs/channelsettingsdialog.cpp
// Phiên bản: 1.7 (Cải tiến giao diện theo yêu cầu)

#include "channelsettingsdialog.h"
#include "ui_channelsettingsdialog.h"
#include "../repositories/ichannelrepository.h"
#include "../appsettings.h"
#include <QSettings>
#include <QKeyEvent>
#include <QCompleter> // Thêm header cho QCompleter
#include <QStringListModel> // Thêm header cho QStringListModel

ChannelSettingsDialog::ChannelSettingsDialog(IChannelRepository* channelRepo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChannelSettingsDialog),
    m_channelRepo(channelRepo)
{
    ui->setupUi(this);

    if (!m_channelRepo) {
        // Vô hiệu hóa dialog nếu không có db manager
        ui->defaultsGroupBox->setEnabled(false);
        return;
    }

    // Cài đặt bộ lọc sự kiện để xử lý phím Tab
    ui->defaultTagsTextEdit->installEventFilter(this);
    ui->defaultSubTagsTextEdit->installEventFilter(this);

    // Thiết lập lại thứ tự Tab một cách tường minh theo yêu cầu
    setTabOrder(ui->channelSelectionComboBox, ui->defaultCategoryLineEdit);
    setTabOrder(ui->defaultCategoryLineEdit, ui->defaultPlaylistLineEdit);
    setTabOrder(ui->defaultPlaylistLineEdit, ui->defaultTagsTextEdit);
    setTabOrder(ui->defaultTagsTextEdit, ui->defaultSubTagsTextEdit);

    // Nạp danh sách kênh vào ComboBox
    m_channels = m_channelRepo->loadAllChannels();
    QStringList channelNames;
    for(const auto& channel : m_channels) {
        ui->channelSelectionComboBox->addItem(channel.name, channel.id);
        channelNames.append(channel.name);
    }

    // Thiết lập chức năng tìm kiếm cho ComboBox
    QCompleter* completer = new QCompleter(channelNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains); // Gợi ý các kết quả chứa chuỗi tìm kiếm
    ui->channelSelectionComboBox->setCompleter(completer);
    
    // Ngăn người dùng thêm mục mới vào combobox
    ui->channelSelectionComboBox->setInsertPolicy(QComboBox::NoInsert);


    connect(ui->channelSelectionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ChannelSettingsDialog::onChannelSelectionChanged);
    connect(this, &QDialog::accepted, this, &ChannelSettingsDialog::onSaveSettings);

    // Tải cài đặt cho kênh được chọn đầu tiên
    if (!m_channels.isEmpty()) {
        onChannelSelectionChanged(0);
    }
}

ChannelSettingsDialog::~ChannelSettingsDialog()
{
    delete ui;
}

bool ChannelSettingsDialog::eventFilter(QObject *obj, QEvent *event)
{
    // Kiểm tra nếu sự kiện là một phím nhấn trên một trong hai ô QPlainTextEdit
    if ((obj == ui->defaultTagsTextEdit || obj == ui->defaultSubTagsTextEdit) && event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        // Nếu phím được nhấn là phím Tab
        if (keyEvent->key() == Qt::Key_Tab)
        {
            // Di chuyển tiêu điểm sang widget tiếp theo trong thứ tự tab
            focusNextChild();
            // Trả về true để báo rằng sự kiện đã được xử lý và ngăn không cho ký tự Tab được chèn vào
            return true;
        }
    }
    // Đối với tất cả các sự kiện khác, sử dụng trình xử lý mặc định
    return QDialog::eventFilter(obj, event);
}


void ChannelSettingsDialog::onChannelSelectionChanged(int index)
{
    // Bỏ qua nếu index không hợp lệ (ví dụ: khi đang tìm kiếm mà chưa chọn)
    if (index < 0) return;

    int channelId = ui->channelSelectionComboBox->itemData(index).toInt();
    if (channelId > 0) {
        loadSettingsForChannel(channelId);
    }
}

void ChannelSettingsDialog::onSaveSettings()
{
    int channelId = ui->channelSelectionComboBox->currentData().toInt();
    if (channelId <= 0) return;

    QSettings& settings = getAppSettings();
    settings.beginGroup(QString("Channel/%1").arg(channelId));

    // Lưu tất cả cài đặt
    settings.setValue("DefaultCategory", ui->defaultCategoryLineEdit->text());
    settings.setValue("DefaultPlaylist", ui->defaultPlaylistLineEdit->text());
    settings.setValue("DefaultTags", ui->defaultTagsTextEdit->toPlainText());
    settings.setValue("DefaultSubTags", ui->defaultSubTagsTextEdit->toPlainText());
    
    settings.endGroup();
}

void ChannelSettingsDialog::loadSettingsForChannel(int channelId)
{
    QSettings& settings = getAppSettings();
    settings.beginGroup(QString("Channel/%1").arg(channelId));

    // Tải tất cả cài đặt
    ui->defaultCategoryLineEdit->setText(settings.value("DefaultCategory", "").toString());
    ui->defaultPlaylistLineEdit->setText(settings.value("DefaultPlaylist", "").toString());
    ui->defaultTagsTextEdit->setPlainText(settings.value("DefaultTags", "").toString());
    ui->defaultSubTagsTextEdit->setPlainText(settings.value("DefaultSubTags", "").toString());

    settings.endGroup();
}
