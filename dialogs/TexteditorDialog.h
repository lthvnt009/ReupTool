// Phiên bản: 1.0
// Vị trí: /src/dialogs/texteditordialog.h

#pragma once

#include <QDialog>
#include <QString>

namespace Ui {
class TextEditorDialog;
}

class TextEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TextEditorDialog(const QString& initialText, QWidget *parent = nullptr);
    ~TextEditorDialog();

    QString getText() const;

private:
    Ui::TextEditorDialog *ui;
};
