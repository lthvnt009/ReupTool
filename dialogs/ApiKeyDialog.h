// Vị trí: /src/dialogs/ApiKeyDialog.h
// Phiên bản: 1.1 (Hoàn thiện)
#pragma once
#include <QDialog>
#include "../models.h"

namespace Ui { class ApiKeyDialog; }

class ApiKeyDialog : public QDialog {
    Q_OBJECT
public:
    explicit ApiKeyDialog(QWidget *parent = nullptr);
    explicit ApiKeyDialog(const ApiKey& key, QWidget *parent = nullptr);
    ~ApiKeyDialog();

    ApiKey getApiKey() const;

private:
    void setApiKey(const ApiKey& key);
    Ui::ApiKeyDialog *ui;
};
