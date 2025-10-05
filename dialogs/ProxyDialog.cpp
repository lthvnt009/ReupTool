// Vị trí: /src/dialogs/ProxyDialog.cpp
// Phiên bản: 1.1 (Hoàn thiện)
#include "proxydialog.h"
#include "ui_proxydialog.h"

ProxyDialog::ProxyDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::ProxyDialog) {
    ui->setupUi(this);
    setWindowTitle("Thêm Proxy");
}

ProxyDialog::ProxyDialog(const Proxy &proxy, QWidget *parent) :
    QDialog(parent), ui(new Ui::ProxyDialog) {
    ui->setupUi(this);
    setWindowTitle("Sửa Proxy");
    setProxy(proxy);
}

ProxyDialog::~ProxyDialog() {
    delete ui;
}

Proxy ProxyDialog::getProxy() const
{
    Proxy p;
    p.host = ui->hostLineEdit->text();
    p.port = ui->portSpinBox->value();
    p.username = ui->usernameLineEdit->text();
    p.password = ui->passwordLineEdit->text();
    return p;
}

void ProxyDialog::setProxy(const Proxy &proxy)
{
    ui->hostLineEdit->setText(proxy.host);
    ui->portSpinBox->setValue(proxy.port);
    ui->usernameLineEdit->setText(proxy.username);
    ui->passwordLineEdit->setText(proxy.password);
}
