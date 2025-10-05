// Vị trí: /src/models/ProxyModel.h
// Phiên bản: 1.1 (Hoàn thiện)
#pragma once
#include <QAbstractTableModel>
#include <QList>
#include "../models.h"

class ProxyModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProxyModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setProxies(const QList<Proxy>& proxies);
    const QList<Proxy>& getProxies() const;

private:
    QList<Proxy> m_proxies;
};
