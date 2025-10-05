// Vị trí: /src/mainwindow.h
// Phiên bản: 4.2 (Tích hợp logic Font)

#pragma once

#include <QMainWindow>
#include <memory>
#include <QCloseEvent>
#include <QFont> // Thêm

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Forward declarations
class DatabaseManager;
class DownloadQueueService;
class LibraryService;
class SettingsService;
class ChannelController;
class DataInputController;
class DownloadController;
class ManualUploadController;
class LibraryController;
class SettingsController;

class ChannelManagerWidget;
class DataInputWidget;
class DownloadWidget;
class ManualUploadWidget;
class LibraryWidget;
class SettingsWidget;

class QLabel;
class QPushButton;
class QTimer;
class QProgressBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(
        std::unique_ptr<DatabaseManager> dbManager,
        QWidget *parent = nullptr
    );
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

public slots:
    void applyTheme(const QString& theme);
    void applyFont(const QFont& font);

private slots:
    void showUndoNotification(const QString& message);
    void hideUndoNotification();
    void showTaskProgress(int percentage);
    void hideTaskProgress(const QString& message = "");

private:
    void createTabWidgets();
    void createServices();
    void createControllers();
    void setupConnections();
    void setupStatusBar();
    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;

    // Services
    std::unique_ptr<DatabaseManager> m_dbManager;
    std::unique_ptr<DownloadQueueService> m_downloadQueueService;
    std::unique_ptr<LibraryService> m_libraryService;
    std::unique_ptr<SettingsService> m_settingsService;

    // Tab Widgets
    ChannelManagerWidget* m_channelManagerWidget;
    DataInputWidget* m_dataInputWidget;
    DownloadWidget* m_downloadWidget;
    ManualUploadWidget* m_manualUploadWidget;
    LibraryWidget* m_libraryWidget;
    SettingsWidget* m_settingsWidget;

    // Controllers
    std::unique_ptr<ChannelController> m_channelController;
    std::unique_ptr<DataInputController> m_dataInputController;
    std::unique_ptr<DownloadController> m_downloadController;
    std::unique_ptr<ManualUploadController> m_manualUploadController;
    std::unique_ptr<LibraryController> m_libraryController;
    std::unique_ptr<SettingsController> m_settingsController;
    
    // Status Bar Widgets
    QLabel* m_undoLabel;
    QPushButton* m_undoButton;
    QTimer* m_undoTimer;
    QProgressBar* m_taskProgressBar;
    QLabel* m_statusMessageLabel;
};
