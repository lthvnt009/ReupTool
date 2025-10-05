// Vị trí: src/widgets/settings/DataSourceWidget.cpp
// Phiên bản: 1.3 (Hoàn thiện)
#include "DataSourceWidget.h"
#include "ui_DataSourceWidget.h"
#include <QButtonGroup>
#include <QHeaderView>

DataSourceWidget::DataSourceWidget(QWidget *parent) :
    QWidget(parent), ui(new Ui::DataSourceWidget) {
    ui->setupUi(this);

    m_fetchStrategyGroup = new QButtonGroup(this);
    m_fetchStrategyGroup->addButton(ui->apiRadioButton);
    m_fetchStrategyGroup->addButton(ui->ytdlpRadioButton);

    m_apiKeyModel = new ApiKeyModel(this);
    ui->youtubeApiTableView->setModel(m_apiKeyModel);
    ui->youtubeApiTableView->horizontalHeader()->setStretchLastSection(true);
    ui->youtubeApiTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->youtubeApiTableView->setSelectionMode(QAbstractItemView::SingleSelection);


    connect(ui->apiRadioButton, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) emit fetchStrategyChanged("API");
    });
    connect(ui->ytdlpRadioButton, &QRadioButton::toggled, this, [this](bool checked){
        if (checked) emit fetchStrategyChanged("yt-dlp");
    });

    connect(ui->addYoutubeApiKeyButton, &QPushButton::clicked, this, &DataSourceWidget::addApiKeyClicked);
    connect(ui->editYoutubeApiKeyButton, &QPushButton::clicked, this, &DataSourceWidget::editApiKeyClicked);
    connect(ui->deleteYoutubeApiKeyButton, &QPushButton::clicked, this, &DataSourceWidget::deleteApiKeyClicked);
}

DataSourceWidget::~DataSourceWidget() {
    delete ui;
}

void DataSourceWidget::setFetchStrategy(const QString &strategy)
{
    if (strategy.toLower() == "api") {
        ui->apiRadioButton->setChecked(true);
    } else {
        ui->ytdlpRadioButton->setChecked(true);
    }
}

QString DataSourceWidget::getFetchStrategy() const
{
    return ui->apiRadioButton->isChecked() ? "API" : "yt-dlp";
}

ApiKeyModel *DataSourceWidget::getApiKeyModel()
{
    return m_apiKeyModel;
}

int DataSourceWidget::getSelectedApiKeyIndex() const
{
    QModelIndexList selected = ui->youtubeApiTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        return -1;
    }
    return selected.first().row();
}
