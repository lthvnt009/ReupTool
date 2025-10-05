// Phiên bản: 2.0 (Tái cấu trúc)
// Vị trí: /src/widgets/datainputwidget.h
// Mô tả: Widget chuyên dụng cho giao diện tab "Nhập Dữ Liệu".

#pragma once

#include <QWidget>

// Forward declarations
namespace Ui { class DataInputWidget; }
class DataInputToolbarWidget;

class DataInputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataInputWidget(QWidget *parent = nullptr);
    ~DataInputWidget();

    Ui::DataInputWidget* getUi() const;
    DataInputToolbarWidget* getToolbarWidget() const;

private:
    Ui::DataInputWidget *ui;
    DataInputToolbarWidget *m_toolbarWidget;
};
