// Vị trí: /src/models/ApiKeyModel.cpp
// Phiên bản: 1.1 (Hoàn thiện)
#include "apikeymodel.h"
#include <QBrush>
#include <QColor>

ApiKeyModel::ApiKeyModel(QObject *parent) : QAbstractTableModel(parent) {}

int ApiKeyModel::rowCount(const QModelIndex &) const {
    return m_apiKeys.count();
}

int ApiKeyModel::columnCount(const QModelIndex &) const {
    return 3; // Tên, Key, Trạng thái
}

QVariant ApiKeyModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_apiKeys.count()) {
        return QVariant();
    }

    const ApiKey& key = m_apiKeys[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return key.alias;
            case 1: return "****" + key.keyValue.right(4); // Che bớt key
            case 2: return key.isValid ? "Hợp lệ" : "Chưa kiểm tra";
        }
    }

    if (role == Qt::ForegroundRole && index.column() == 2) {
        return QBrush(key.isValid ? QColor("green") : QColor("orange"));
    }

    return QVariant();
}

QVariant ApiKeyModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Tên gợi nhớ";
            case 1: return "Khóa API";
            case 2: return "Trạng thái";
        }
    }
    return QVariant();
}

void ApiKeyModel::setApiKeys(const QList<ApiKey> &keys)
{
    beginResetModel();
    m_apiKeys = keys;
    endResetModel();
}

const QList<ApiKey>& ApiKeyModel::getApiKeys() const
{
    return m_apiKeys;
}
