// Phiên bản: 1.0 (Mới)
// Vị trí: /src/widgets/clearablelineedit.h
// Mô tả: Một QLineEdit tùy chỉnh có nút 'x' để xóa nội dung.

#pragma once

#include <QLineEdit>

class QAction;

class ClearableLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    explicit ClearableLineEdit(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTextChanged(const QString &text);

private:
    QAction *m_clearAction;
};
