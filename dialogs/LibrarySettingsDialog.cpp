// Vị trí: /src/dialogs/LibrarySettingsDialog.cpp
// Phiên bản: 1.2 (Sửa lỗi Build)

#include "librarysettingsdialog.h"
#include "ui_librarysettingsdialog.h"
#include "../appsettings.h"
#include <QTableView>
#include <QHeaderView>
#include <QSettings>
#include <QPushButton> // Sửa lỗi: Thêm header bị thiếu

LibrarySettingsDialog::LibrarySettingsDialog(QTableView* tableView, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LibrarySettingsDialog),
    m_tableView(tableView)
{
    ui->setupUi(this);

    createCheckboxes();
    loadSettings();

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &LibrarySettingsDialog::applySettings);
    connect(ui->maintenanceButton, &QPushButton::clicked, this, &LibrarySettingsDialog::openMaintenanceDialogRequested);
}

LibrarySettingsDialog::~LibrarySettingsDialog()
{
    delete ui;
}

void LibrarySettingsDialog::createCheckboxes()
{
    if (!m_tableView || !m_tableView->model()) return;

    auto* model = m_tableView->model();
    for (int i = 0; i < model->columnCount(); ++i) {
        // Lấy header từ model nguồn, không phải header view
        QString headerText = model->headerData(i, Qt::Horizontal).toString();
        
        // Bỏ qua các cột đặc biệt không nên cho người dùng ẩn/hiện
        if (headerText.isEmpty() || headerText == "ID" || headerText == "Link Video" || headerText == "Thao tác" || headerText.contains("✓")) {
            continue;
        }

        QCheckBox* checkbox = new QCheckBox(headerText, this);
        checkbox->setObjectName("col_" + headerText);
        ui->columnsLayout->addWidget(checkbox);
        m_checkboxes.append(checkbox);
    }
}

void LibrarySettingsDialog::loadSettings()
{
    QSettings& settings = getAppSettings();
    QStringList hiddenColumns = settings.value("Library/hiddenColumns").toStringList();

    for (auto* checkbox : m_checkboxes) {
        QString columnName = checkbox->text();
        checkbox->setChecked(!hiddenColumns.contains(columnName));
    }
}

void LibrarySettingsDialog::saveSettings()
{
    QStringList hiddenColumns;
    for (auto* checkbox : m_checkboxes) {
        if (!checkbox->isChecked()) {
            hiddenColumns.append(checkbox->text());
        }
    }
    QSettings& settings = getAppSettings();
    settings.setValue("Library/hiddenColumns", hiddenColumns);
}


void LibrarySettingsDialog::applySettings()
{
    saveSettings();
    emit settingsChanged();
    accept();
}

