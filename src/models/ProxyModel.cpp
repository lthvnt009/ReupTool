// Vị trí: /src/models/ProxyModel.cpp
// Phiên bản: 1.1 (Hoàn thiện)
#include "proxymodel.h"

ProxyModel::ProxyModel(QObject *parent) : QAbstractTableModel(parent) {}

int ProxyModel::rowCount(const QModelIndex &) const {
    return m_proxies.count();
}

int ProxyModel::columnCount(const QModelIndex &) const {
    return 4; // Host, Port, Username, Password
}

QVariant ProxyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_proxies.count()) {
        return QVariant();
    }

    const Proxy& proxy = m_proxies[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return proxy.host;
            case 1: return proxy.port;
            case 2: return proxy.username;
            case 3: return proxy.password.isEmpty() ? "" : "******"; // Che password
        }
    }
    return QVariant();
}

QVariant ProxyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Host";
            case 1: return "Port";
            case 2: return "Username";
            case 3: return "Password";
        }
    }
    return QVariant();
}

void ProxyModel::setProxies(const QList<Proxy> &proxies)
{
    beginResetModel();
    m_proxies = proxies;
    endResetModel();
}

const QList<Proxy>& ProxyModel::getProxies() const
{
    return m_proxies;
}
