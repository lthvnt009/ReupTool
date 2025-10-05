// Vị trí: /src/dialogs/BatchEditTagsDialog.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Dialog cho phép chỉnh sửa tags hàng loạt.

#pragma once

#include <QDialog>

namespace Ui {
class BatchEditTagsDialog;
}

class BatchEditTagsDialog : public QDialog
{
    Q_OBJECT

public:
    enum Action { Add, Remove, Replace };

    explicit BatchEditTagsDialog(QWidget *parent = nullptr);
    ~BatchEditTagsDialog();

    Action getAction() const;
    QString getTag1() const;
    QString getTag2() const;

private slots:
    void onActionChanged(int index);

private:
    Ui::BatchEditTagsDialog *ui;
};
