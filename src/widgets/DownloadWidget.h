// Vị trí: /src/widgets/downloadwidget.h
// Phiên bản: 2.0 (Tái cấu trúc)

#pragma once

#include <QWidget>

// Forward declarations
namespace Ui {
class DownloadWidget;
}
class DownloadToolbarWidget;
class DownloadControlBarWidget;
class DownloadOptionsWidget;


class DownloadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadWidget(QWidget *parent = nullptr);
    ~DownloadWidget();

    Ui::DownloadWidget* getUi() const;

    // Cung cấp getters để Controller có thể truy cập các widget con
    DownloadToolbarWidget* getToolbarWidget() const;
    DownloadControlBarWidget* getControlBarWidget() const;
    DownloadOptionsWidget* getOptionsWidget() const;

private:
    Ui::DownloadWidget *ui;
};
