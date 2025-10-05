// Vị trí: /src/dialogs/BatchChangeChannelDialog.cpp
// Phiên bản: 1.0 (Mới)

#include "batchchangechanneldialog.h"
#include "ui_batchchangechanneldialog.h"
#include "../repositories/ichannelrepository.h"
#include "../models.h"

BatchChangeChannelDialog::BatchChangeChannelDialog(IChannelRepository* channelRepo, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchChangeChannelDialog),
    m_channelRepo(channelRepo)
{
    ui->setupUi(this);
    populateChannels();
}

BatchChangeChannelDialog::~BatchChangeChannelDialog()
{
    delete ui;
}

int BatchChangeChannelDialog::getSelectedChannelId() const
{
    return ui->channelComboBox->currentData().toInt();
}

void BatchChangeChannelDialog::populateChannels()
{
    if (!m_channelRepo) return;

    ui->channelComboBox->clear();
    QList<Channel> channels = m_channelRepo->loadAllChannels();
    for (const auto& ch : channels) {
        ui->channelComboBox->addItem(ch.name, ch.id);
    }
}
