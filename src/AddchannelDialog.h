// Phiên bản: 1.0
// Vị trí: /src/addchanneldialog.h

#pragma once

#include <QDialog>
#include <QString>

namespace Ui {
class AddChannelDialog;
}

class AddChannelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddChannelDialog(QWidget *parent = nullptr);
    ~AddChannelDialog();

    // Lấy dữ liệu người dùng đã nhập
    QString channelName() const;
    QString channelLink() const;

    // Thiết lập dữ liệu ban đầu cho dialog (dùng cho chức năng Sửa)
    void setChannelData(const QString &name, const QString &link);

private:
    Ui::AddChannelDialog *ui;
};
