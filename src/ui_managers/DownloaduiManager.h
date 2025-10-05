// Vị trí: /src/ui_managers/downloaduimanager.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Lớp chuyên trách quản lý giao diện cho Tab Tải Video.

#pragma once

#include <QObject>
#include <QStringList>
#include <QModelIndex>
#include "../models.h"

// Forward declarations
class DownloadWidget;
class DownloadQueueModel;
class QStandardItemModel;
class QPoint;

class DownloadUIManager : public QObject
{
    Q_OBJECT

public:
    enum class ItemAction {
        TogglePause,
        Rename,
        Refresh,
        Delete,
        Skip,
        OpenFolder,
        CopyUrl,
        CopyError
    };
    Q_ENUM(ItemAction)

    explicit DownloadUIManager(DownloadWidget* widget, DownloadQueueModel* model, QObject* parent = nullptr);
    ~DownloadUIManager();

    DownloadOptions getCurrentOptionsFromUi();

public slots:
    void onQueueChanged();
    void onItemChanged(int row);
    void displayLog(const QString& logContent);
    void applySettings();

signals:
    void addFromClipboardClicked();
    void startAllClicked();
    void stopAllClicked();
    void settingsClicked();
    void getFromLibraryClicked();
    void itemActionRequested(int row, ItemAction action);
    void logViewRequested(int row);

private slots:
    void onBackToListClicked();
    void onItemDoubleClicked(const QModelIndex& index);
    void onContextMenuRequested(const QPoint& pos);
    void onDownloadTypeChanged(const QString& type);
    void onSavePathComboBoxEdited();
    void onSavePathTextChanged(const QString& text);
    void onBrowseSavePathClicked();
    void onClearSavePathHistory();
    void onCutVideoCheckBoxToggled(bool checked);
    void onCustomCommandCheckBoxToggled(bool checked);
    void onClearCustomCommandClicked();

private:
    void setupConnections();
    void loadSettings();
    void saveSettings();
    void populateSavePathHistory();

    DownloadWidget* m_widget;
    DownloadQueueModel* m_queueModel;

    QStringList m_savePathHistory;
    QString m_lastValidSavePath;
    QModelIndex m_contextMenuIndex;
};
