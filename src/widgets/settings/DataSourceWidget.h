// Vị trí: src/widgets/settings/DataSourceWidget.h
// Phiên bản: 1.2 (Hoàn thiện)
#pragma once
#include <QWidget>
#include "../../models/ApiKeyModel.h"

namespace Ui { class DataSourceWidget; }
class QButtonGroup;

class DataSourceWidget : public QWidget {
    Q_OBJECT
public:
    explicit DataSourceWidget(QWidget *parent = nullptr);
    ~DataSourceWidget();

    void setFetchStrategy(const QString& strategy);
    QString getFetchStrategy() const;
    ApiKeyModel* getApiKeyModel();
    int getSelectedApiKeyIndex() const;

signals:
    void fetchStrategyChanged(const QString& strategy);
    void addApiKeyClicked();
    void editApiKeyClicked();
    void deleteApiKeyClicked();

private:
    Ui::DataSourceWidget *ui;
    QButtonGroup* m_fetchStrategyGroup;
    ApiKeyModel* m_apiKeyModel;
};
