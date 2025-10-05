// Vị trí: src/widgets/settings/NetworkSettingsWidget.cpp
// Phiên bản: 1.1 (Hoàn thiện)
#include "networksettingswidget.h"
#include "ui_networksettingswidget.h"
#include <QHeaderView>

NetworkSettingsWidget::NetworkSettingsWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::NetworkSettingsWidget) {
    ui->setupUi(this);

    m_proxyModel = new ProxyModel(this);
    ui->proxyTableView->setModel(m_proxyModel);
    ui->proxyTableView->horizontalHeader()->setStretchLastSection(true);
    ui->proxyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->proxyTableView->setSelectionMode(QAbstractItemView::SingleSelection);

    // Tạm thời vô hiệu hóa phần cookie từ trình duyệt
    ui->browserComboBox->setEnabled(false);
    ui->profileComboBox->setEnabled(false);
    ui->importCookieButton->setEnabled(false);

    connect(ui->addProxyButton, &QPushButton::clicked, this, &NetworkSettingsWidget::addProxyClicked);
    connect(ui->editProxyButton, &QPushButton::clicked, this, &NetworkSettingsWidget::editProxyClicked);
    connect(ui->deleteProxyButton, &QPushButton::clicked, this, &NetworkSettingsWidget::deleteProxyClicked);
    connect(ui->importCookieFileButton, &QPushButton::clicked, this, &NetworkSettingsWidget::importCookieFileClicked);
}

NetworkSettingsWidget::~NetworkSettingsWidget() {
    delete ui;
}

ProxyModel *NetworkSettingsWidget::getProxyModel()
{
    return m_proxyModel;
}

int NetworkSettingsWidget::getSelectedProxyIndex() const
{
    QModelIndexList selected = ui->proxyTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }
    return selected.first().row();
}

void NetworkSettingsWidget::setCookieStatus(const QString &status)
{
    ui->cookieStatusLabel->setText(status);
}
