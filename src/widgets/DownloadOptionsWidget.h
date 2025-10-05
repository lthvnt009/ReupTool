// Vị trí: /src/widgets/downloadoptionswidget.h
// Phiên bản: 1.0 (Mới)
#pragma once

#include <QWidget>

namespace Ui {
class DownloadOptionsWidget;
}

class DownloadOptionsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadOptionsWidget(QWidget *parent = nullptr);
    ~DownloadOptionsWidget();

    Ui::DownloadOptionsWidget* getUi() const;

private:
    Ui::DownloadOptionsWidget *ui;
};
