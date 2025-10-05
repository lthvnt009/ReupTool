// Vị trí: /src/controllers/channelcontroller.cpp
// Phiên bản: 6.4 (Sửa lỗi không cập nhật view sau khi import)

#include "channelcontroller.h"
#include "../widgets/channelmanagerwidget.h"
#include "../widgets/channellistwidget.h"
#include "ui_channellistwidget.h"
#include "../widgets/channeltoolbarwidget.h"
#include "ui_channeltoolbarwidget.h"
#include "../widgets/channelstatisticswidget.h"
#include "../repositories/ichannelrepository.h"
#include "../models/channelmodel.h"
#include "../addchanneldialog.h"
#include "../dialogs/channelsettingsdialog.h"
#include "../services/fileimportservice.h"

#include <QStandardItem>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QHeaderView>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFileDialog>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QPainter>

// Delegate tùy chỉnh để xử lý toàn bộ việc vẽ item trong bảng
class CustomChannelDelegate : public QStyledItemDelegate
{
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        QStyleOptionViewItem viewOption(option);
        initStyleOption(&viewOption, index);

        // --- SỬA LỖI BÔI ĐEN (TRIỆT ĐỂ) ---
        // Xóa trạng thái focus và hover để không vẽ hiệu ứng không mong muốn
        if (viewOption.state & QStyle::State_HasFocus) {
            viewOption.state &= ~QStyle::State_HasFocus;
        }
        if (viewOption.state & QStyle::State_MouseOver) {
            viewOption.state &= ~QStyle::State_MouseOver;
        }

        // --- SỬA LỖI 2 CHECKBOX & CĂN GIỮA (TRIỆT ĐỂ) ---
        // Nếu là cột "Ẩn", chúng ta sẽ tự vẽ hoàn toàn
        if (index.column() == 4) {
            const QWidget* widget = option.widget;
            QStyle* style = widget ? widget->style() : QApplication::style();

            // 1. Chỉ vẽ nền của item (bao gồm cả màu khi được chọn)
            style->drawPrimitive(QStyle::PE_PanelItemViewItem, &viewOption, painter, widget);

            // 2. Vẽ checkbox tùy chỉnh ở giữa
            const Qt::CheckState checkState = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
            QStyleOptionButton checkboxOption;
            checkboxOption.state = (checkState == Qt::Checked) ? QStyle::State_On : QStyle::State_Off;
            checkboxOption.state |= QStyle::State_Enabled;
            checkboxOption.rect = style->subElementRect(QStyle::SE_CheckBoxIndicator, &viewOption, widget);
            checkboxOption.rect.moveCenter(option.rect.center());
            style->drawControl(QStyle::CE_CheckBox, &checkboxOption, painter, widget);
        } else {
            // Đối với tất cả các cột khác, chỉ cần vẽ bình thường với trạng thái đã được chỉnh sửa
            QStyledItemDelegate::paint(painter, viewOption, index);
        }
    }
};


ChannelController::ChannelController(ChannelManagerWidget* widget, IChannelRepository* channelRepo, QObject* parent)
    : QObject(parent)
    , m_widget(widget)
    , m_channelRepo(channelRepo)
    , m_importService(new FileImportService(this))
    , m_channelModel(new ChannelModel(0, 5, this)) // STT, ID, Tên Kênh, Link Kênh, Ẩn
{
    if (!m_widget) {
        qCritical() << "ChannelManagerWidget is null!";
        return;
    }

    auto listUi = m_widget->getListWidget()->getUi();
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();

    m_channelModel->setHorizontalHeaderLabels({"STT", "ID", "Tên Kênh", "Link Kênh", "Ẩn"});
    listUi->channelTableView->setModel(m_channelModel);
    listUi->channelTableView->setColumnHidden(1, true); // Ẩn cột ID

    listUi->channelTableView->setAlternatingRowColors(true);
    listUi->channelTableView->setShowGrid(true);
    listUi->channelTableView->verticalHeader()->setVisible(false);
    listUi->channelTableView->setWordWrap(false);
    listUi->channelTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Tắt focus policy để không có khung nét đứt khi chọn
    listUi->channelTableView->setFocusPolicy(Qt::NoFocus);

    listUi->channelTableView->setDragEnabled(true);
    listUi->channelTableView->setAcceptDrops(true);
    listUi->channelTableView->setDropIndicatorShown(true);
    listUi->channelTableView->setDefaultDropAction(Qt::MoveAction);
    listUi->channelTableView->setDragDropMode(QAbstractItemView::InternalMove);

    listUi->channelTableView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    listUi->channelTableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    auto* header = listUi->channelTableView->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Interactive);
    header->setSectionResizeMode(3, QHeaderView::Stretch);
    header->setSectionResizeMode(4, QHeaderView::Fixed);

    listUi->channelTableView->verticalHeader()->setDefaultSectionSize(28);

    listUi->channelTableView->setColumnWidth(0, 40);
    listUi->channelTableView->setColumnWidth(4, 50);

    // Áp dụng delegate tùy chỉnh cho toàn bộ bảng
    listUi->channelTableView->setItemDelegate(new CustomChannelDelegate(listUi->channelTableView));

    auto* selectionModel = listUi->channelTableView->selectionModel();
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &ChannelController::onChannelSelectionChanged);
    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &ChannelController::updateButtonStates);
    connect(m_channelModel, &ChannelModel::itemChanged, this, &ChannelController::onItemChanged);
    
    connect(m_channelModel, &ChannelModel::orderChanged, this, &ChannelController::onOrderChanged, Qt::QueuedConnection);

    connect(toolbarUi->addToolButton, &QToolButton::clicked, this, &ChannelController::onAddChannel);
    connect(toolbarUi->importToolButton, &QToolButton::clicked, this, &ChannelController::onImportChannels);
    connect(toolbarUi->editToolButton, &QToolButton::clicked, this, &ChannelController::onEditChannel);
    connect(toolbarUi->deleteToolButton, &QToolButton::clicked, this, &ChannelController::onDeleteChannel);
    connect(toolbarUi->upToolButton, &QToolButton::clicked, this, &ChannelController::onMoveUp);
    connect(toolbarUi->downToolButton, &QToolButton::clicked, this, &ChannelController::onMoveDown);
    connect(toolbarUi->settingsToolButton, &QToolButton::clicked, this, &ChannelController::onSettingsClicked);

    connect(m_importService, &FileImportService::errorOccurred, this, [this](const QString& msg){
        QMessageBox::critical(m_widget, "Lỗi Nhập File", msg);
    });

    loadChannelsFromDb();
}

void ChannelController::loadChannelsFromDb()
{
    auto listUi = m_widget->getListWidget()->getUi();
    auto* selectionModel = listUi->channelTableView->selectionModel();

    selectionModel->blockSignals(true);

    // Sửa lỗi: Tạm thời ngắt kết nối slot 'onItemChanged' để tránh các tác dụng phụ
    // khi đang nạp lại dữ liệu, nhưng vẫn cho phép model phát tín hiệu để view cập nhật.
    disconnect(m_channelModel, &ChannelModel::itemChanged, this, &ChannelController::onItemChanged);

    m_channelModel->removeRows(0, m_channelModel->rowCount());
    if (!m_channelRepo) {
        // Kết nối lại slot trước khi thoát
        connect(m_channelModel, &ChannelModel::itemChanged, this, &ChannelController::onItemChanged);
        return;
    }

    m_channels = m_channelRepo->loadAllChannels();
    for (int i = 0; i < m_channels.size(); ++i)
    {
        const auto& ch = m_channels.at(i);
        QList<QStandardItem *> newRow;
        
        // Căn giữa cột STT
        auto sttItem = new QStandardItem(QString::number(i + 1));
        sttItem->setTextAlignment(Qt::AlignCenter);
        newRow.append(sttItem);
        
        auto idItem = new QStandardItem();
        idItem->setData(ch.id, Qt::DisplayRole);
        newRow.append(idItem);
        newRow.append(new QStandardItem(ch.name));
        newRow.append(new QStandardItem(ch.link));

        auto* checkItem = new QStandardItem();
        checkItem->setCheckable(true);
        checkItem->setCheckState(ch.isHidden ? Qt::Checked : Qt::Unchecked);
        newRow.append(checkItem);
        
        for(int col = 0; col < newRow.size(); ++col) {
            if (col != 4) newRow.at(col)->setFlags(newRow.at(col)->flags() & ~Qt::ItemIsEditable);
        }
        if (ch.isHidden) {
            QFont font; font.setItalic(true);
            QBrush brush(Qt::lightGray);
            for(auto* item : newRow) {
                item->setData(font, Qt::FontRole);
                item->setData(brush, Qt::ForegroundRole);
            }
        }
        m_channelModel->appendRow(newRow);
    }
    
    // Yêu cầu bảng tính lại chiều cao các hàng sau khi đã nạp dữ liệu
    listUi->channelTableView->resizeRowsToContents();
    
    // Kết nối lại slot 'onItemChanged' sau khi đã hoàn tất việc nạp dữ liệu
    connect(m_channelModel, &ChannelModel::itemChanged, this, &ChannelController::onItemChanged);

    selectionModel->blockSignals(false);
    
    updateButtonStates();
    // Kích hoạt sự kiện selectionChanged một cách thủ công để cập nhật panel thống kê
    onChannelSelectionChanged(selectionModel->selection(), QItemSelection());
}


void ChannelController::onAddChannel()
{
    AddChannelDialog dialog(m_widget);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString name = dialog.channelName();
        QString link = dialog.channelLink();
        if (name.isEmpty() || link.isEmpty()) {
            QMessageBox::warning(m_widget, "Thiếu thông tin", "Vui lòng nhập đầy đủ tên và link kênh.");
            return;
        }
        if (m_channelRepo->channelLinkExists(link)) {
            QMessageBox::warning(m_widget, "Kênh đã tồn tại", "Kênh với link này đã có trong cơ sở dữ liệu.");
            return;
        }
        if (m_channelRepo->addChannel(name, link)) {
            loadChannelsFromDb();
            emit channelsChanged();
        } else {
            QMessageBox::critical(m_widget, "Lỗi", "Không thể thêm kênh vào cơ sở dữ liệu.");
        }
    }
}

void ChannelController::onImportChannels()
{
    QString filePath = QFileDialog::getOpenFileName(m_widget, "Chọn file để nhập", "", "Tất cả file được hỗ trợ (*.csv *.xlsx);;CSV Files (*.csv);;Excel Files (*.xlsx)");
    if (filePath.isEmpty()) return;
    QList<Channel> channelsToImport;
    if (filePath.endsWith(".csv", Qt::CaseInsensitive)) channelsToImport = m_importService->importChannelsFromCsv(filePath);
    else if (filePath.endsWith(".xlsx", Qt::CaseInsensitive)) channelsToImport = m_importService->importChannelsFromXlsx(filePath);
    else { QMessageBox::warning(m_widget, "Định dạng không hỗ trợ", "Vui lòng chọn file có định dạng .csv hoặc .xlsx."); return; }
    if (channelsToImport.isEmpty()) { QMessageBox::information(m_widget, "Thông báo", "Không tìm thấy kênh hợp lệ nào trong file."); return; }
    int successCount = 0, skippedCount = 0;
    for (const auto& ch : channelsToImport) {
        if (m_channelRepo->channelLinkExists(ch.link)) { skippedCount++; continue; }
        if (m_channelRepo->addChannel(ch.name, ch.link)) successCount++;
    }
    QMessageBox::information(m_widget, "Hoàn tất", QString("Nhập hoàn tất!\n\n- Thêm mới: %1 kênh\n- Bỏ qua (do trùng link): %2 kênh").arg(successCount).arg(skippedCount));
    if (successCount > 0) { loadChannelsFromDb(); emit channelsChanged(); }
}


void ChannelController::onEditChannel()
{
    QModelIndex currentIndex = m_widget->getListWidget()->getUi()->channelTableView->currentIndex();
    if (!currentIndex.isValid()) return;
    int channelId = m_channelModel->item(currentIndex.row(), 1)->data(Qt::DisplayRole).toInt();
    Channel channelToEdit = m_channelRepo->getChannelById(channelId);
    if(channelToEdit.id == -1) return;
    AddChannelDialog dialog(m_widget);
    dialog.setChannelData(channelToEdit.name, channelToEdit.link);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString newName = dialog.channelName(), newLink = dialog.channelLink();
        if (newName.isEmpty() || newLink.isEmpty()) { QMessageBox::warning(m_widget, "Thiếu thông tin", "Vui lòng nhập đầy đủ tên và link kênh."); return; }
        if (newLink != channelToEdit.link && m_channelRepo->channelLinkExists(newLink)) { QMessageBox::warning(m_widget, "Kênh đã tồn tại", "Kênh với link mới này đã có trong cơ sở dữ liệu."); return; }
        channelToEdit.name = newName; channelToEdit.link = newLink;
        if (m_channelRepo->updateChannel(channelToEdit)) { loadChannelsFromDb(); emit channelsChanged(); }
        else { QMessageBox::critical(m_widget, "Lỗi", "Không thể cập nhật kênh trong cơ sở dữ liệu."); }
    }
}

void ChannelController::onDeleteChannel()
{
    auto* selectionModel = m_widget->getListWidget()->getUi()->channelTableView->selectionModel();
    if (!selectionModel->hasSelection()) return;
    QModelIndexList selectedRows = selectionModel->selectedRows();
    QString question = (selectedRows.count() == 1) ? "Bạn có chắc chắn muốn xóa kênh này không?" : QString("Bạn có chắc chắn muốn xóa %1 kênh đã chọn không?").arg(selectedRows.count());
    question += "\nTất cả video trong hàng đợi của các kênh này cũng sẽ bị xóa.";
    if (QMessageBox::question(m_widget, "Xác nhận xóa", question, QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QList<int> idsToDelete;
        for (const QModelIndex& index : selectedRows) idsToDelete.append(m_channelModel->item(index.row(), 1)->data(Qt::DisplayRole).toInt());
        int successCount = 0;
        for (int id : idsToDelete) if (m_channelRepo->deleteChannel(id)) successCount++;
        if (successCount > 0) { loadChannelsFromDb(); emit channelsChanged(); }
        if (successCount < idsToDelete.count()) QMessageBox::critical(m_widget, "Lỗi", "Không thể xóa một hoặc nhiều kênh khỏi cơ sở dữ liệu.");
    }
}

void ChannelController::onMoveUp()
{
    QModelIndex currentIndex = m_widget->getListWidget()->getUi()->channelTableView->currentIndex();
    if (!currentIndex.isValid()) return;
    int row = currentIndex.row();
    if (row > 0) {
        m_channels.move(row, row - 1);
        if (m_channelRepo->updateChannelsOrder(m_channels)) {
            loadChannelsFromDb();
            m_widget->getListWidget()->getUi()->channelTableView->selectRow(row - 1);
            emit channelsChanged();
        }
    }
}

void ChannelController::onMoveDown()
{
    QModelIndex currentIndex = m_widget->getListWidget()->getUi()->channelTableView->currentIndex();
    if (!currentIndex.isValid()) return;
    int row = currentIndex.row();
    if (row < m_channels.size() - 1) {
        m_channels.move(row, row + 1);
        if (m_channelRepo->updateChannelsOrder(m_channels)) {
            loadChannelsFromDb();
            m_widget->getListWidget()->getUi()->channelTableView->selectRow(row + 1);
            emit channelsChanged();
        }
    }
}


void ChannelController::onSettingsClicked()
{
    ChannelSettingsDialog dialog(m_channelRepo, m_widget);
    if(dialog.exec() == QDialog::Accepted) emit channelsChanged();
}

void ChannelController::onChannelSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    auto statsWidget = m_widget->getStatisticsWidget();
    if (!statsWidget) return;

    auto selectionModel = m_widget->getListWidget()->getUi()->channelTableView->selectionModel();
    int selectionCount = selectionModel->selectedRows().count();

    if (selectionCount == 1) {
        // Lấy hàng được chọn
        int row = selectionModel->selectedRows().first().row();
        QStandardItem* idItem = m_channelModel->item(row, 1);
        if (idItem) {
            int channelId = idItem->data(Qt::DisplayRole).toInt();
            // Tìm kênh tương ứng trong danh sách đã tải
            for (const auto& ch : qAsConst(m_channels)) {
                if (ch.id == channelId) {
                    statsWidget->updateForChannel(ch);
                    return;
                }
            }
        }
    } else if (selectionCount > 1) {
        // Nếu chọn nhiều kênh
        statsWidget->updateForMultipleChannels(selectionCount);
    } else {
        // Nếu không có kênh nào được chọn
        statsWidget->clearStatistics();
    }
}


void ChannelController::onItemChanged(QStandardItem *item)
{
    if(item->column() == 4) {
        int row = item->row();
        int channelId = m_channelModel->item(row, 1)->data(Qt::DisplayRole).toInt();
        bool isHidden = (item->checkState() == Qt::Checked);
        QFont font; font.setItalic(isHidden);
        QBrush brush = isHidden ? QBrush(Qt::lightGray) : QBrush(Qt::black);
        for (int col = 0; col < m_channelModel->columnCount(); ++col) {
            auto* currentItem = m_channelModel->item(row, col);
            if(currentItem) {
                 currentItem->setData(font, Qt::FontRole);
                 currentItem->setData(brush, Qt::ForegroundRole);
            }
        }
        if (m_channelRepo->setChannelHidden(channelId, isHidden)) {
            for(auto& ch : m_channels) {
                if(ch.id == channelId) { ch.isHidden = isHidden; break; }
            }
            emit channelsChanged();
        } else {
            QMessageBox::warning(m_widget, "Lỗi", "Không thể cập nhật trạng thái ẩn cho kênh.");
            loadChannelsFromDb();
        }
    }
}

void ChannelController::onOrderChanged()
{
    QList<Channel> newOrder;
    QHash<int, Channel> channelMap;
    for(const auto& ch : m_channels) channelMap[ch.id] = ch;

    for (int i = 0; i < m_channelModel->rowCount(); ++i) {
        QStandardItem* idItem = m_channelModel->item(i, 1);
        if (idItem) {
            int channelId = idItem->data(Qt::DisplayRole).toInt();
            if (channelMap.contains(channelId)) {
                newOrder.append(channelMap.value(channelId));
            }
        }
    }

    if (newOrder.size() == m_channels.size()) {
        m_channels = newOrder;
        if(m_channelRepo->updateChannelsOrder(m_channels)) {
            m_channelModel->blockSignals(true);
            for (int i = 0; i < m_channelModel->rowCount(); ++i) {
                m_channelModel->item(i, 0)->setText(QString::number(i + 1));
            }
            m_channelModel->blockSignals(false);
            emit channelsChanged();
        } else {
            QMessageBox::critical(m_widget, "Lỗi", "Không thể lưu thứ tự mới vào cơ sở dữ liệu. Tải lại danh sách gốc.");
            loadChannelsFromDb();
        }
    } else {
        QMessageBox::critical(m_widget, "Lỗi nghiêm trọng", "Xảy ra lỗi không đồng bộ khi sắp xếp. Tải lại danh sách để đảm bảo an toàn.");
        loadChannelsFromDb();
    }
}


void ChannelController::updateButtonStates()
{
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    auto* selectionModel = m_widget->getListWidget()->getUi()->channelTableView->selectionModel();
    bool hasSelection = selectionModel->hasSelection();
    int selectionCount = selectionModel->selectedRows().count();
    toolbarUi->deleteToolButton->setEnabled(hasSelection);
    toolbarUi->editToolButton->setEnabled(selectionCount == 1);
    int currentRow = -1;
    if (selectionCount == 1) currentRow = selectionModel->currentIndex().row();
    toolbarUi->upToolButton->setEnabled(selectionCount == 1 && currentRow > 0);
    toolbarUi->downToolButton->setEnabled(selectionCount == 1 && currentRow < m_channels.size() - 1);
}
