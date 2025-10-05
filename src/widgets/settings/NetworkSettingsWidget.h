// Vị trí: src/widgets/settings/NetworkSettingsWidget.h
// Phiên bản: 1.1 (Hoàn thiện)
#pragma once
#include <QWidget>
#include "../../models/ProxyModel.h"

namespace Ui { class NetworkSettingsWidget; }

class NetworkSettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit NetworkSettingsWidget(QWidget *parent = nullptr);
    ~NetworkSettingsWidget();

    ProxyModel* getProxyModel();
    int getSelectedProxyIndex() const;
    void setCookieStatus(const QString& status);


signals:
    void addProxyClicked();
    void editProxyClicked();
    void deleteProxyClicked();
    void importCookieFileClicked();


private:
    Ui::NetworkSettingsWidget *ui;
    ProxyModel* m_proxyModel;
};
