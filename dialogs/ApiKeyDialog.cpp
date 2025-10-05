// Vị trí: /src/dialogs/ApiKeyDialog.cpp
// Phiên bản: 1.1 (Hoàn thiện)
#include "apikeydialog.h"
#include "ui_apikeydialog.h"

ApiKeyDialog::ApiKeyDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::ApiKeyDialog) {
    ui->setupUi(this);
    setWindowTitle("Thêm API Key");
}

ApiKeyDialog::ApiKeyDialog(const ApiKey &key, QWidget *parent) :
    QDialog(parent), ui(new Ui::ApiKeyDialog) {
    ui->setupUi(this);
    setWindowTitle("Sửa API Key");
    setApiKey(key);
}


ApiKeyDialog::~ApiKeyDialog() {
    delete ui;
}

ApiKey ApiKeyDialog::getApiKey() const
{
    ApiKey key;
    key.alias = ui->aliasLineEdit->text();
    key.keyValue = ui->apiKeyLineEdit->text();
    return key;
}

void ApiKeyDialog::setApiKey(const ApiKey &key)
{
    ui->aliasLineEdit->setText(key.alias);
    ui->apiKeyLineEdit->setText(key.keyValue);
}
