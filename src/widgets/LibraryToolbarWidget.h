// Vị trí: /src/widgets/LibraryToolbarWidget.h
// Phiên bản: 1.5 (Sửa lỗi Build - Dependency Injection)
// Mô tả: Module giao diện thanh công cụ cho tab Thư viện.

#pragma once

#include <QWidget>
#include <QMenu>
#include "../models/FilterCriteria.h"

namespace Ui {
class LibraryToolbarWidget;
}

// Forward declarations
class IChannelRepository;

class LibraryToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LibraryToolbarWidget(QWidget *parent = nullptr);
    ~LibraryToolbarWidget();

    FilterCriteria getCurrentCriteria() const;

public slots:
    void setChannelRepository(IChannelRepository* channelRepo); // Phương thức mới
    void resetFilters();
    void onSelectionChanged(int count);

signals:
    void importClicked();
    void exportClicked();
    void settingsClicked();
    void filterRequested();
    void resetRequested();
    void batchEditTagsClicked();
    void batchChangeChannelClicked();
    void batchDeleteClicked();

private slots:
    void updateChannelList();

private:
    void setupBatchActionsMenu();

    Ui::LibraryToolbarWidget *ui;
    IChannelRepository* m_channelRepo; // Sẽ được set qua phương thức mới
    QMenu* m_batchActionsMenu;
};

