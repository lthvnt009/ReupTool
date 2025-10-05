// Vị trí: /src/widgets/downloadtoolbarwidget.h
// Phiên bản: 1.0 (Mới)
#pragma once

#include <QWidget>

namespace Ui {
class DownloadToolbarWidget;
}

class DownloadToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadToolbarWidget(QWidget *parent = nullptr);
    ~DownloadToolbarWidget();

    Ui::DownloadToolbarWidget* getUi() const;

private:
    Ui::DownloadToolbarWidget *ui;
};
