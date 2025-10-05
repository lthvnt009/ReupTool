// Phiên bản: 1.0 (Mới)
// Vị trí: /src/widgets/manualuploadwidget.h
// Mô tả: Widget chuyên dụng cho giao diện tab "Up Video Thủ Công".

#pragma once

#include <QWidget>

namespace Ui {
class ManualUploadWidget;
}

class ManualUploadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ManualUploadWidget(QWidget *parent = nullptr);
    ~ManualUploadWidget();

    Ui::ManualUploadWidget* getUi() const;

private:
    Ui::ManualUploadWidget *ui;
};
