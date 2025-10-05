// Phiên bản: 1.0 (Mới)
// Vị trí: /src/widgets/clearablelineedit.cpp

#include "clearablelineedit.h"
#include <QAction>
#include <QStyle>
#include <QResizeEvent>

ClearableLineEdit::ClearableLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    m_clearAction = addAction(QIcon(":/qt-project.org/styles/commonstyle/images/standardbutton-cancel-16.png"), QLineEdit::TrailingPosition);
    m_clearAction->setVisible(false);

    // Sử dụng stylesheet để làm cho icon hơi mờ đi
    setStyleSheet("QLineEdit QToolButton { border: none; background: transparent; opacity: 0.6; }");

    connect(m_clearAction, &QAction::triggered, this, &ClearableLineEdit::clear);
    connect(this, &QLineEdit::textChanged, this, &ClearableLineEdit::onTextChanged);
}

void ClearableLineEdit::resizeEvent(QResizeEvent *event)
{
    QLineEdit::resizeEvent(event);
    // Có thể thêm logic điều chỉnh vị trí nút xóa nếu cần
}

void ClearableLineEdit::onTextChanged(const QString &text)
{
    m_clearAction->setVisible(!text.isEmpty());
}
