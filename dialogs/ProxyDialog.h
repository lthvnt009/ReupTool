// Vị trí: /src/dialogs/ProxyDialog.h
// Phiên bản: 1.1 (Hoàn thiện)
#pragma once
#include <QDialog>
#include "../models.h"

namespace Ui { class ProxyDialog; }

class ProxyDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProxyDialog(QWidget *parent = nullptr);
    explicit ProxyDialog(const Proxy& proxy, QWidget* parent = nullptr);
    ~ProxyDialog();

    Proxy getProxy() const;

private:
    void setProxy(const Proxy& proxy);
    Ui::ProxyDialog *ui;
};
