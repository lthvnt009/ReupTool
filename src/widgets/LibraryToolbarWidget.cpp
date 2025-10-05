// Vị trí: /src/widgets/LibraryToolbarWidget.cpp
// Phiên bản: 1.5 (Sửa lỗi Build - Dependency Injection)

#include "librarytoolbarwidget.h"
#include "ui_librarytoolbarwidget.h"
#include "../repositories/ichannelrepository.h"
#include <QCompleter>
#include <QStringListModel>

LibraryToolbarWidget::LibraryToolbarWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryToolbarWidget),
    m_channelRepo(nullptr) // Khởi tạo là null
{
    ui->setupUi(this);

    // Cài đặt bộ lọc trạng thái
    ui->statusFilterComboBox->addItem("Tất cả trạng thái", "");
    ui->statusFilterComboBox->addItem("Đã Upload", "Uploaded");
    ui->statusFilterComboBox->addItem("Đã tải xong", "Completed");
    ui->statusFilterComboBox->addItem("Đang chờ", "Pending");
    ui->statusFilterComboBox->addItem("Lỗi tải", "Failed");
    
    setupBatchActionsMenu();

    connect(ui->importButton, &QToolButton::clicked, this, &LibraryToolbarWidget::importClicked);
    connect(ui->exportButton, &QToolButton::clicked, this, &LibraryToolbarWidget::exportClicked);
    connect(ui->settingsButton, &QToolButton::clicked, this, &LibraryToolbarWidget::settingsClicked);
    connect(ui->filterButton, &QPushButton::clicked, this, &LibraryToolbarWidget::filterRequested);
    connect(ui->resetButton, &QPushButton::clicked, this, &LibraryToolbarWidget::resetRequested);
}

LibraryToolbarWidget::~LibraryToolbarWidget()
{
    delete ui;
}

void LibraryToolbarWidget::setChannelRepository(IChannelRepository* channelRepo)
{
    m_channelRepo = channelRepo;
    if (m_channelRepo) {
        updateChannelList(); // Nạp dữ liệu kênh sau khi có repository
    }
}

FilterCriteria LibraryToolbarWidget::getCurrentCriteria() const
{
    FilterCriteria criteria;
    criteria.searchTerm = ui->searchLineEdit->text();
    
    // Lấy ID kênh từ dữ liệu của ComboBox
    int currentIndex = ui->channelFilterComboBox->currentIndex();
    if(currentIndex != -1) {
        criteria.channelId = ui->channelFilterComboBox->itemData(currentIndex).toInt();
    } else {
        criteria.channelId = -1;
    }

    criteria.status = ui->statusFilterComboBox->currentData().toString();
    return criteria;
}

void LibraryToolbarWidget::resetFilters()
{
    ui->searchLineEdit->clear();
    ui->channelFilterComboBox->setCurrentIndex(0);
    ui->statusFilterComboBox->setCurrentIndex(0);
}

void LibraryToolbarWidget::onSelectionChanged(int count)
{
    ui->batchActionsButton->setEnabled(count > 0);
    ui->batchActionsButton->setText(QString("Hành động (%1)").arg(count));
}


void LibraryToolbarWidget::updateChannelList()
{
    if (!m_channelRepo) return;

    QStringList channelNames;
    QList<Channel> channels = m_channelRepo->loadAllChannels();

    ui->channelFilterComboBox->blockSignals(true);
    ui->channelFilterComboBox->clear();
    ui->channelFilterComboBox->addItem("Tất cả kênh", -1); // Giá trị -1 cho "Tất cả"

    for (const auto& channel : channels) {
        ui->channelFilterComboBox->addItem(channel.name, channel.id);
        channelNames.append(channel.name);
    }
    ui->channelFilterComboBox->blockSignals(false);
    
    // Cài đặt completer
    QCompleter* completer = new QCompleter(channelNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setFilterMode(Qt::MatchContains);
    ui->channelFilterComboBox->setCompleter(completer);
}

void LibraryToolbarWidget::setupBatchActionsMenu()
{
    m_batchActionsMenu = new QMenu(this);
    QAction* editTagsAction = m_batchActionsMenu->addAction("Chỉnh sửa Tags...");
    QAction* changeChannelAction = m_batchActionsMenu->addAction("Chuyển Kênh...");
    m_batchActionsMenu->addSeparator();
    QAction* deleteAction = m_batchActionsMenu->addAction("Xóa các mục đã chọn");

    ui->batchActionsButton->setMenu(m_batchActionsMenu);
    ui->batchActionsButton->setEnabled(false); // Vô hiệu hóa ban đầu

    connect(editTagsAction, &QAction::triggered, this, &LibraryToolbarWidget::batchEditTagsClicked);
    connect(changeChannelAction, &QAction::triggered, this, &LibraryToolbarWidget::batchChangeChannelClicked);
    connect(deleteAction, &QAction::triggered, this, &LibraryToolbarWidget::batchDeleteClicked);
}

