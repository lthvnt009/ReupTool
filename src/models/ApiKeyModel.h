// Vị trí: /src/models/ApiKeyModel.h
// Phiên bản: 1.1 (Hoàn thiện)
#pragma once
#include <QAbstractTableModel>
#include <QList>
#include "../models.h"

class ApiKeyModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ApiKeyModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void setApiKeys(const QList<ApiKey>& keys);
    const QList<ApiKey>& getApiKeys() const;

private:
    QList<ApiKey> m_apiKeys;
};
