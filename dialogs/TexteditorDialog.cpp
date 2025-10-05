// Phiên bản: 1.0
// Vị trí: /src/dialogs/texteditordialog.cpp

#include "texteditordialog.h"
#include "ui_texteditordialog.h"

TextEditorDialog::TextEditorDialog(const QString& initialText, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TextEditorDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->setPlainText(initialText);
}

TextEditorDialog::~TextEditorDialog()
{
    delete ui;
}

QString TextEditorDialog::getText() const
{
    return ui->plainTextEdit->toPlainText();
}
