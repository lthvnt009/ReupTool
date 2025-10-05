// Phiên bản: 1.0 (Mới)
// Vị trí: /src/widgets/datainputtoolbarwidget.h

#pragma once

#include <QWidget>

namespace Ui {
class DataInputToolbarWidget;
}

class DataInputToolbarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataInputToolbarWidget(QWidget *parent = nullptr);
    ~DataInputToolbarWidget();

    Ui::DataInputToolbarWidget* getUi() const;

private:
    Ui::DataInputToolbarWidget *ui;
};
