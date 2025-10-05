// Vị trí: src/widgets/settings/DataSettingsWidget.h
// Phiên bản: 1.1 (Thêm signals và methods)
#pragma once
#include <QWidget>
namespace Ui { class DataSettingsWidget; }

class DataSettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit DataSettingsWidget(QWidget *parent = nullptr);
    ~DataSettingsWidget();

    void setDbPath(const QString& path);
    void setAutoSaveEnabled(bool enabled);
    void setAutoSaveInterval(int minutes);

signals:
    void changeDbPathClicked();
    void backupClicked();
    void restoreClicked();
    void autoSaveToggled(bool enabled);
    void autoSaveIntervalChanged(int minutes);

private:
    Ui::DataSettingsWidget *ui;
};
