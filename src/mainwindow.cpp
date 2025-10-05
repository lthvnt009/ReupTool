// Vị trí: /src/mainwindow.cpp
// Phiên bản: 4.3 (Kết nối signal khởi động lại)

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "databasemanager.h"
#include "controllers/channelcontroller.h"
#include "controllers/datainputcontroller.h"
#include "controllers/downloadcontroller.h"
#include "controllers/manualuploadcontroller.h"
#include "controllers/librarycontroller.h"
#include "controllers/settingscontroller.h"
#include "services/downloadqueueservice.h"
#include "services/libraryservice.h"
#include "services/settingsservice.h"
#include "widgets/channelmanagerwidget.h"
#include "ui_channelmanagerwidget.h"
#include "widgets/channellistwidget.h"
#include "ui_channellistwidget.h"
#include "widgets/datainputwidget.h"
#include "widgets/downloadwidget.h"
#include "widgets/manualuploadwidget.h"
#include "widgets/librarywidget.h"
#include "widgets/settingswidget.h"
#include "appsettings.h"

#include <QMessageBox>
#include <QSettings>
#include <QSplitter>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QProgressBar>
#include <QStyleFactory>
#include <QDebug>
#include <QPalette>
#include <QColor>

MainWindow::MainWindow(
    std::unique_ptr<DatabaseManager> dbManager,
    QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_dbManager(std::move(dbManager))
{
    ui->setupUi(this);
    setupStatusBar();

    if (!m_dbManager || !m_dbManager->isDbOpen()) {
         QMessageBox::critical(this, "Lỗi CSDL", "Không thể kết nối CSDL. Chương trình sẽ bị vô hiệu hóa.");
         ui->centralwidget->setEnabled(false);
         return;
    }

    createServices();
    createTabWidgets();
    createControllers();
    setupConnections();

    loadSettings();

    // Tải dữ liệu ban đầu cho các tab
    m_channelController->loadChannelsFromDb();
    m_dataInputController->reloadDataAndUI();
    m_libraryController->refreshData();
}

MainWindow::~MainWindow()
{
    // unique_ptr sẽ tự động dọn dẹp
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings(); 

    // Sử dụng SettingsService để kiểm tra cài đặt
    bool warn = m_settingsService->loadRememberWindowState();

    if (warn && m_dataInputController->hasUnsavedData()) {
        auto reply = QMessageBox::question(this,
                                           "Xác nhận thoát",
                                           "Vẫn còn ngày trống chưa được điền video trong Tab Nhập Dữ Liệu.\n"
                                           "Bạn có chắc chắn muốn thoát không?",
                                           QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    
    event->accept();
}

void MainWindow::applyTheme(const QString& theme)
{
    if (theme == "Dark") {
        qApp->setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        qApp->setPalette(darkPalette);
    } else { // Light hoặc System
        qApp->setStyle(QStyleFactory::create("Fusion"));
        qApp->setPalette(this->style()->standardPalette());
    }
}

void MainWindow::applyFont(const QFont& font)
{
    QApplication::setFont(font);
}


void MainWindow::createTabWidgets()
{
    m_channelManagerWidget = new ChannelManagerWidget(
        m_dbManager->getChannelRepository(), 
        m_dbManager->getVideoRepository(), 
        this
    );
    m_dataInputWidget = new DataInputWidget(this);
    m_downloadWidget = new DownloadWidget(this);
    m_manualUploadWidget = new ManualUploadWidget(this);
    m_libraryWidget = new LibraryWidget(this);
    m_settingsWidget = new SettingsWidget(this);


    ui->tab_ChannelManager->layout()->addWidget(m_channelManagerWidget);
    ui->tab_DataInput->layout()->addWidget(m_dataInputWidget);
    ui->tab_VideoDownloader->layout()->addWidget(m_downloadWidget);
    ui->tab_ManualUpload->layout()->addWidget(m_manualUploadWidget);
    ui->tab_Library->layout()->addWidget(m_libraryWidget);
    ui->tab_Settings->layout()->addWidget(m_settingsWidget);
}

void MainWindow::createServices()
{
     m_downloadQueueService = std::make_unique<DownloadQueueService>(this);
     m_libraryService = std::make_unique<LibraryService>(m_dbManager->getVideoRepository(), m_dbManager->getChannelRepository(), this);
     m_settingsService = std::make_unique<SettingsService>(this);
}

void MainWindow::createControllers()
{
    m_channelController = std::make_unique<ChannelController>(m_channelManagerWidget, m_dbManager->getChannelRepository(), this);
    m_dataInputController = std::make_unique<DataInputController>(m_dataInputWidget, m_dbManager->getChannelRepository(), m_dbManager->getVideoRepository(), this);
    m_downloadController = std::make_unique<DownloadController>(m_downloadWidget, m_dbManager->getVideoRepository(), m_downloadQueueService.get(), this);
    m_manualUploadController = std::make_unique<ManualUploadController>(m_manualUploadWidget, m_dbManager->getChannelRepository(), m_dbManager->getVideoRepository(), this);
    
    m_libraryController = std::make_unique<LibraryController>(
        m_libraryWidget->getToolbarWidget(),
        m_libraryWidget->getPaginationWidget(),
        m_libraryWidget->getTableView(),
        m_libraryService.get(),
        m_dbManager->getChannelRepository(),
        this
    );

    m_settingsController = std::make_unique<SettingsController>(m_settingsWidget, this);
}

void MainWindow::setupConnections()
{
    connect(m_channelController.get(), &ChannelController::channelsChanged, m_dataInputController.get(), &DataInputController::reloadDataAndUI);
    connect(m_dataInputController.get(), &DataInputController::startDownloadRequested, m_downloadController.get(), &DownloadController::onDownloadRequested);
    connect(m_downloadQueueService.get(), &DownloadQueueService::downloadCompleted, m_manualUploadController.get(), &ManualUploadController::refreshCompletedVideos);
    connect(m_downloadQueueService.get(), &DownloadQueueService::downloadProgressUpdated, m_dataInputController.get(), &DataInputController::onDownloadProgressUpdated);
    
    connect(ui->tabWidget, &QTabWidget::currentChanged, [this](int index){
        if (ui->tabWidget->widget(index) == ui->tab_ManualUpload) {
            m_manualUploadController->refreshCompletedVideos();
        }
        if (ui->tabWidget->widget(index) == ui->tab_Library) {
            m_libraryController->refreshData();
        }
    });

    connect(m_libraryController.get(), &LibraryController::requeueDownloadRequested, m_downloadController.get(), &DownloadController::onRequeueDownloadRequested);
    connect(m_libraryController.get(), &LibraryController::requeueDownloadRequested, this, [this](const QList<int>&){
        ui->tabWidget->setCurrentWidget(ui->tab_VideoDownloader);
    });

    connect(m_libraryService.get(), &LibraryService::videosModified, m_dataInputController.get(), &DataInputController::onVideosModified);

    connect(m_libraryController.get(), &LibraryController::taskStarted, this, [this](){ showTaskProgress(0); });
    connect(m_libraryController.get(), &LibraryController::taskProgress, this, &MainWindow::showTaskProgress);
    connect(m_libraryController.get(), &LibraryController::taskFinished, this, &MainWindow::hideTaskProgress);
    connect(m_libraryController.get(), &LibraryController::showUndoAction, this, &MainWindow::showUndoNotification);
    connect(m_undoButton, &QPushButton::clicked, m_libraryController.get(), &LibraryController::onUndoDeleteTriggered);

    // --- Connection cho Settings ---
    connect(m_settingsController.get(), &SettingsController::themeChanged, this, &MainWindow::applyTheme);
    connect(m_settingsController.get(), &SettingsController::fontChanged, this, &MainWindow::applyFont);
    connect(m_settingsController.get(), &SettingsController::restartRequired, this, &MainWindow::close);
}

void MainWindow::setupStatusBar()
{
    m_undoLabel = new QLabel(this);
    m_undoButton = new QPushButton("Hoàn tác", this);
    m_undoTimer = new QTimer(this);
    m_undoTimer->setSingleShot(true);
    m_undoTimer->setInterval(7000);

    m_taskProgressBar = new QProgressBar(this);
    m_taskProgressBar->setMaximumSize(150, 19);
    m_taskProgressBar->setTextVisible(false);

    m_statusMessageLabel = new QLabel(this);

    statusBar()->addPermanentWidget(m_statusMessageLabel);
    statusBar()->addPermanentWidget(m_taskProgressBar);
    statusBar()->addPermanentWidget(m_undoLabel);
    statusBar()->addPermanentWidget(m_undoButton);

    m_undoLabel->hide();
    m_undoButton->hide();
    m_taskProgressBar->hide();

    connect(m_undoTimer, &QTimer::timeout, this, &MainWindow::hideUndoNotification);
}

void MainWindow::showUndoNotification(const QString& message)
{
    hideTaskProgress();
    m_undoLabel->setText(message);
    m_undoLabel->show();
    m_undoButton->show();
    m_undoTimer->start();
}

void MainWindow::hideUndoNotification()
{
    m_undoLabel->hide();
    m_undoButton->hide();
}

void MainWindow::showTaskProgress(int percentage)
{
    hideUndoNotification();
    m_taskProgressBar->show();
    m_taskProgressBar->setValue(percentage);
    m_statusMessageLabel->hide();
}

void MainWindow::hideTaskProgress(const QString& message)
{
    m_taskProgressBar->hide();
    m_statusMessageLabel->setText(message);
    m_statusMessageLabel->show();

    if (!message.isEmpty()) {
        QTimer::singleShot(5000, m_statusMessageLabel, &QLabel::clear);
    }
}


void MainWindow::loadSettings()
{
    // Tải và áp dụng theme
    applyTheme(m_settingsService->loadTheme());
    
    // Tải và áp dụng font
    applyFont(m_settingsService->loadAppFont());

    // Tải và áp dụng kích thước cửa sổ
    QByteArray geometry = m_settingsService->loadWindowGeometry();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    } else {
        resize(1024, 768); // Kích thước mặc định
    }
    
    // Tải các cài đặt cũ khác nếu cần
    QSettings& settings = getAppSettings();
    QByteArray splitterState = settings.value("MainWindow/channelManagerSplitterState").toByteArray();
    if (!splitterState.isEmpty()) {
        m_channelManagerWidget->getSplitter()->restoreState(splitterState);
    }
    
    QByteArray headerState = settings.value("MainWindow/channelListHeaderState").toByteArray();
    if (!headerState.isEmpty()) {
        m_channelManagerWidget->getListWidget()->getUi()->channelTableView->horizontalHeader()->restoreState(headerState);
    }
}

void MainWindow::saveSettings()
{
    // Lưu kích thước cửa sổ qua service
    m_settingsService->saveWindowGeometry(saveGeometry());
    
    // Lưu các cài đặt cũ khác
    QSettings& settings = getAppSettings();
    settings.setValue("MainWindow/channelManagerSplitterState", m_channelManagerWidget->getSplitter()->saveState());
    settings.setValue("MainWindow/channelListHeaderState", m_channelManagerWidget->getListWidget()->getUi()->channelTableView->horizontalHeader()->saveState());
}

