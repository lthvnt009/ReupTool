// Phiên bản: 1.0
// Vị trí: /src/addchanneldialog.cpp

#include "addchanneldialog.h"
#include "ui_addchanneldialog.h"

AddChannelDialog::AddChannelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddChannelDialog)
{
    ui->setupUi(this);
}

AddChannelDialog::~AddChannelDialog()
{
    delete ui;
}

QString AddChannelDialog::channelName() const
{
    return ui->channelNameEdit->text();
}

QString AddChannelDialog::channelLink() const
{
    return ui->channelLinkEdit->text();
}

void AddChannelDialog::setChannelData(const QString &name, const QString &link)
{
    ui->channelNameEdit->setText(name);
    ui->channelLinkEdit->setText(link);
}
