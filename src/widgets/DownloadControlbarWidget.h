// Vị trí: /src/widgets/downloadcontrolbarwidget.h
// Phiên bản: 1.0 (Mới)
#pragma once

#include <QWidget>

namespace Ui {
class DownloadControlBarWidget;
}

class DownloadControlBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadControlBarWidget(QWidget *parent = nullptr);
    ~DownloadControlBarWidget();

    Ui::DownloadControlBarWidget* getUi() const;

signals:
    void addClicked();
    void startClicked();
    void stopClicked();

private:
    Ui::DownloadControlBarWidget *ui;
};
