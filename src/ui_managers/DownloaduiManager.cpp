// Vị trí: /src/ui_managers/downloaduimanager.cpp
// Phiên bản: 1.2 (Sử dụng AppSettings)

#include "downloaduimanager.h"
#include "../widgets/downloadwidget.h"
#include "ui_downloadwidget.h"
#include "../widgets/downloadtoolbarwidget.h"
#include "ui_downloadtoolbarwidget.h"
#include "../widgets/downloadcontrolbarwidget.h"
#include "ui_downloadcontrolbarwidget.h"
#include "../widgets/downloadoptionswidget.h"
#include "ui_downloadoptionswidget.h"
#include "../models/downloadqueuemodel.h"
#include "../appsettings.h"

#include <QSettings>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QTreeView>
#include <QMenu>
#include <QInputDialog>
#include <QTimer>
#include <QColor>
#include <QBrush>
#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QMessageBox>

DownloadUIManager::DownloadUIManager(DownloadWidget* widget, DownloadQueueModel* model, QObject* parent)
    : QObject(parent),
      m_widget(widget),
      m_queueModel(model),
      m_lastValidSavePath("")
{
    if (!m_widget || !m_queueModel) {
        qCritical() << "DownloadUIManager: Widget or model is null!";
        return;
    }

    m_widget->getUi()->downloadsTableView->setModel(m_queueModel);

    // Khởi tạo các ComboBox
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    toolbarUi->downloadTypeComboBox->addItems({"Video", "Audio"});
    
    auto optionsUi = m_widget->getOptionsWidget()->getUi();
    optionsUi->thumbnailComboBox->addItems({"Bỏ qua", "jpg", "png", "webp"});
    optionsUi->subtitleComboBox->addItems({"Bỏ qua", "Tự động", "Tiếng Anh"});

    setupConnections();

    onDownloadTypeChanged(toolbarUi->downloadTypeComboBox->currentText());
    loadSettings();
    applySettings();
}

DownloadUIManager::~DownloadUIManager()
{
    saveSettings();
}

void DownloadUIManager::setupConnections()
{
    auto ui = m_widget->getUi();
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    auto controlBarUi = m_widget->getControlBarWidget()->getUi();
    auto optionsUi = m_widget->getOptionsWidget()->getUi();

    // Toolbar
    connect(toolbarUi->settingsButton, &QToolButton::clicked, this, &DownloadUIManager::settingsClicked);
    connect(toolbarUi->getFromLibraryCheckBox, &QCheckBox::clicked, this, &DownloadUIManager::getFromLibraryClicked);
    connect(toolbarUi->downloadTypeComboBox, &QComboBox::currentTextChanged, this, &DownloadUIManager::onDownloadTypeChanged);
    connect(toolbarUi->savePathComboBox->lineEdit(), &QLineEdit::editingFinished, this, &DownloadUIManager::onSavePathComboBoxEdited);
    connect(toolbarUi->savePathComboBox, &QComboBox::currentTextChanged, this, &DownloadUIManager::onSavePathTextChanged);

    // Control Bar
    connect(controlBarUi->addButton, &QPushButton::clicked, this, &DownloadUIManager::addFromClipboardClicked);
    connect(controlBarUi->startButton, &QPushButton::clicked, this, &DownloadUIManager::startAllClicked);
    connect(controlBarUi->stopButton, &QPushButton::clicked, this, &DownloadUIManager::stopAllClicked);

    // Options
    connect(optionsUi->cutVideoCheckBox, &QCheckBox::toggled, this, &DownloadUIManager::onCutVideoCheckBoxToggled);
    connect(optionsUi->useCustomCommandCheckBox, &QCheckBox::toggled, this, &DownloadUIManager::onCustomCommandCheckBoxToggled);
    connect(optionsUi->clearCustomCommandButton, &QToolButton::clicked, this, &DownloadUIManager::onClearCustomCommandClicked);

    // Main View
    connect(ui->backToListButton, &QPushButton::clicked, this, &DownloadUIManager::onBackToListClicked);
    connect(ui->downloadsTableView, &QTableView::doubleClicked, this, &DownloadUIManager::onItemDoubleClicked);
    connect(ui->downloadsTableView, &QTableView::customContextMenuRequested, this, &DownloadUIManager::onContextMenuRequested);
}

void DownloadUIManager::onQueueChanged() { m_queueModel->updateQueue(); }
void DownloadUIManager::onItemChanged(int row) { m_queueModel->updateItem(row); }

void DownloadUIManager::displayLog(const QString& logContent)
{
    auto ui = m_widget->getUi();
    ui->logTextEdit->setPlainText(logContent);
    ui->mainViewStackedWidget->setCurrentIndex(1);
}

void DownloadUIManager::applySettings()
{
    // Cài đặt liên quan đến UI sẽ được áp dụng ở đây
}

DownloadOptions DownloadUIManager::getCurrentOptionsFromUi()
{
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    auto optionsUi = m_widget->getOptionsWidget()->getUi();

    DownloadOptions opts;
    opts.downloadType = toolbarUi->downloadTypeComboBox->currentText();
    opts.format = toolbarUi->formatComboBox->currentText();
    opts.quality = toolbarUi->qualityComboBox->currentText();
    opts.savePath = toolbarUi->savePathComboBox->currentText();
    opts.thumbnailFormat = optionsUi->thumbnailComboBox->currentText();
    opts.subtitleLang = optionsUi->subtitleComboBox->currentText();
    opts.downloadPlaylist = optionsUi->downloadPlaylistCheckBox->isChecked();
    opts.writeDescription = optionsUi->writeDescriptionCheckBox->isChecked();
    opts.cutVideo = optionsUi->cutVideoCheckBox->isChecked();
    if(opts.cutVideo) opts.cutTimeRange = optionsUi->cutTimeLineEdit->text();
    opts.useCustomCommand = optionsUi->useCustomCommandCheckBox->isChecked();
    if(opts.useCustomCommand) opts.customCommand = optionsUi->customCommandComboBox->currentText();

    QSettings& settings = getAppSettings();
    settings.beginGroup("DownloadSettings");
    opts.sponsorBlockRemove = settings.value("sponsorBlockRemove").toStringList();
    opts.sponsorBlockMark = settings.value("sponsorBlockMark").toStringList();
    if(opts.downloadPlaylist) {
        opts.outputTemplate = settings.value("playlistTemplate", "%(playlist_index)s - %(title)s.%(ext)s").toString();
        if(settings.value("createPlaylistFolder", true).toBool()){
            opts.outputTemplate = QDir::toNativeSeparators("%(playlist)s/" + opts.outputTemplate);
        }
    } else {
        opts.outputTemplate = settings.value("videoTemplate", "%(title)s.%(ext)s").toString();
    }
    settings.endGroup();

    return opts;
}

void DownloadUIManager::onBackToListClicked() { m_widget->getUi()->mainViewStackedWidget->setCurrentIndex(0); }
void DownloadUIManager::onItemDoubleClicked(const QModelIndex& index)
{
    if (index.isValid()) emit logViewRequested(index.row());
}

void DownloadUIManager::onContextMenuRequested(const QPoint& pos)
{
    m_contextMenuIndex = m_widget->getUi()->downloadsTableView->indexAt(pos);
    if (!m_contextMenuIndex.isValid()) return;

    int selectedRow = m_contextMenuIndex.row();
    const DownloadItem& selectedItem = m_queueModel->getQueue()->at(selectedRow);

    QMenu contextMenu;
    QAction* startPauseAction = nullptr;
    if (selectedItem.status == DownloadStatus::Downloading) startPauseAction = contextMenu.addAction("Tạm dừng");
    else if (selectedItem.status != DownloadStatus::Completed) startPauseAction = contextMenu.addAction("Bắt đầu");
    
    QAction* renameAction = contextMenu.addAction("Đặt tên...");
    QAction* refreshAction = contextMenu.addAction("Làm mới");
    contextMenu.addSeparator();
    QAction* deleteAction = contextMenu.addAction("Xóa");
    QAction* skipAction = contextMenu.addAction("Bỏ qua");
    contextMenu.addSeparator();
    QAction* openFolderAction = contextMenu.addAction("Mở thư mục");
    QAction* copyUrlAction = contextMenu.addAction("Sao chép URL");
    QAction* copyErrorAction = contextMenu.addAction("Sao chép Lỗi");
    
    renameAction->setEnabled(selectedItem.status != DownloadStatus::Downloading && selectedItem.status != DownloadStatus::Completed);
    refreshAction->setEnabled(selectedItem.status == DownloadStatus::Error);
    openFolderAction->setEnabled(selectedItem.status == DownloadStatus::Completed && !selectedItem.localFilePath.isEmpty());
    copyErrorAction->setEnabled(selectedItem.status == DownloadStatus::Error && !selectedItem.errorMessage.isEmpty());

    QAction* selectedAction = contextMenu.exec(m_widget->getUi()->downloadsTableView->viewport()->mapToGlobal(pos));

    if (selectedAction == startPauseAction) emit itemActionRequested(selectedRow, ItemAction::TogglePause);
    else if (selectedAction == renameAction) emit itemActionRequested(selectedRow, ItemAction::Rename);
    else if (selectedAction == refreshAction) emit itemActionRequested(selectedRow, ItemAction::Refresh);
    else if (selectedAction == deleteAction) emit itemActionRequested(selectedRow, ItemAction::Delete);
    else if (selectedAction == skipAction) emit itemActionRequested(selectedRow, ItemAction::Skip);
    else if (selectedAction == openFolderAction) emit itemActionRequested(selectedRow, ItemAction::OpenFolder);
    else if (selectedAction == copyUrlAction) emit itemActionRequested(selectedRow, ItemAction::CopyUrl);
    else if (selectedAction == copyErrorAction) emit itemActionRequested(selectedRow, ItemAction::CopyError);
}

void DownloadUIManager::onDownloadTypeChanged(const QString& type)
{
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    toolbarUi->formatComboBox->clear();
    toolbarUi->qualityComboBox->clear();

    if(type == "Video") {
        toolbarUi->formatComboBox->addItems({"MP4", "MKV", "AVI", "WebM"});
        toolbarUi->qualityComboBox->addItems({"Best", "4K", "2K", "1080p", "720p"});
    } else { // Audio
        toolbarUi->formatComboBox->addItems({"m4a", "wav", "mp3", "opus"});
        toolbarUi->qualityComboBox->addItems({"cao", "trung bình", "thấp"});
    }
}

void DownloadUIManager::onSavePathComboBoxEdited()
{
    QString path = m_widget->getToolbarWidget()->getUi()->savePathComboBox->currentText();
    if(!path.isEmpty() && !m_savePathHistory.contains(path) && path != "Thay đổi thư mục lưu..." && path != "Xóa lịch sử đường dẫn") {
        m_savePathHistory.prepend(path);
        while(m_savePathHistory.size() > 10) m_savePathHistory.removeLast();
        populateSavePathHistory();
    }
}

void DownloadUIManager::onSavePathTextChanged(const QString& text)
{
    if (text != "Thay đổi thư mục lưu..." && text != "Xóa lịch sử đường dẫn") {
        m_lastValidSavePath = text;
    }
}

void DownloadUIManager::onBrowseSavePathClicked()
{
    auto toolbarUi = m_widget->getToolbarWidget()->getUi();
    QString currentPath = toolbarUi->savePathComboBox->currentText();
    if(currentPath.isEmpty() || currentPath == "Thay đổi thư mục lưu..." || currentPath == "Xóa lịch sử đường dẫn") {
        currentPath = QDir::homePath();
    }
    QString dir = QFileDialog::getExistingDirectory(m_widget, "Chọn thư mục lưu", currentPath);
    if(!dir.isEmpty()) {
        toolbarUi->savePathComboBox->setCurrentText(dir);
        onSavePathComboBoxEdited();
    }
}

void DownloadUIManager::onClearSavePathHistory()
{
    if(QMessageBox::question(m_widget, "Xác nhận", "Bạn có chắc muốn xóa lịch sử đường dẫn đã lưu?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        m_savePathHistory.clear();
        populateSavePathHistory();
        m_widget->getToolbarWidget()->getUi()->savePathComboBox->setCurrentText("");
    }
}

void DownloadUIManager::onCutVideoCheckBoxToggled(bool checked) { m_widget->getOptionsWidget()->getUi()->cutTimeLineEdit->setEnabled(checked); }
void DownloadUIManager::onCustomCommandCheckBoxToggled(bool checked)
{
    auto optionsUi = m_widget->getOptionsWidget()->getUi();
    optionsUi->customCommandComboBox->setEnabled(checked);
    optionsUi->clearCustomCommandButton->setEnabled(checked);
}
void DownloadUIManager::onClearCustomCommandClicked() { m_widget->getOptionsWidget()->getUi()->customCommandComboBox->lineEdit()->clear(); }

void DownloadUIManager::loadSettings()
{
    QSettings& settings = getAppSettings();
    m_savePathHistory = settings.value("Download/savePathHistory").toStringList();
    populateSavePathHistory();
    QString lastPath = settings.value("Download/lastSavePath").toString();
    m_widget->getToolbarWidget()->getUi()->savePathComboBox->setCurrentText(lastPath);
    m_lastValidSavePath = lastPath;
}

void DownloadUIManager::saveSettings()
{
    QSettings& settings = getAppSettings();
    settings.setValue("Download/savePathHistory", m_savePathHistory);
    settings.setValue("Download/lastSavePath", m_lastValidSavePath);
}

void DownloadUIManager::populateSavePathHistory()
{
    auto comboBox = m_widget->getToolbarWidget()->getUi()->savePathComboBox;
    comboBox->blockSignals(true);
    
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(comboBox->model());
    if(!model) {
        model = new QStandardItemModel(this);
        comboBox->setModel(model);
        QTreeView* view = new QTreeView();
        comboBox->setView(view);
        view->setHeaderHidden(true);
    }
    model->clear();
    
    QStandardItem* browseItem = new QStandardItem("Thay đổi thư mục lưu...");
    browseItem->setData("action_browse", Qt::UserRole);
    model->appendRow(browseItem);

    QStandardItem* clearItem = new QStandardItem("Xóa lịch sử đường dẫn");
    clearItem->setData("action_clear", Qt::UserRole);
    model->appendRow(clearItem);
    
    if (!m_savePathHistory.isEmpty()) {
        QStandardItem* separator = new QStandardItem();
        separator->setFlags(Qt::NoItemFlags);
        separator->setData(QBrush(QColor("gainsboro")), Qt::BackgroundRole);
        separator->setSizeHint(QSize(0, 2));
        model->appendRow(separator);
        if(auto view = qobject_cast<QTreeView*>(comboBox->view())) {
             view->setFirstColumnSpanned(separator->index().row(), view->rootIndex(), true);
        }
    }
    
    for(const QString& path : m_savePathHistory) {
        model->appendRow(new QStandardItem(path));
    }
    
    comboBox->setCurrentText(m_lastValidSavePath);
    comboBox->blockSignals(false);

    static bool isConnected = false;
    if(!isConnected) {
        connect(comboBox, QOverload<int>::of(&QComboBox::activated), this, [this, comboBox](int index){
            if (index < 0 || index >= comboBox->model()->rowCount()) return;
            auto model = qobject_cast<QStandardItemModel*>(comboBox->model());
            QString action = model->item(index)->data(Qt::UserRole).toString();

            comboBox->setCurrentText(m_lastValidSavePath);

            if(action == "action_browse") QTimer::singleShot(0, this, &DownloadUIManager::onBrowseSavePathClicked);
            else if (action == "action_clear") QTimer::singleShot(0, this, &DownloadUIManager::onClearSavePathHistory);
        });
        isConnected = true;
    }
}
