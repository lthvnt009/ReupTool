// Vị trí: src/widgets/settings/SecuritySettingsWidget.h
// Phiên bản: 1.1 (Hoàn thiện)
#pragma once
#include <QWidget>
namespace Ui { class SecuritySettingsWidget; }

class SecuritySettingsWidget : public QWidget {
    Q_OBJECT
public:
    explicit SecuritySettingsWidget(QWidget *parent = nullptr);
    ~SecuritySettingsWidget();

signals:
    void setDbPasswordClicked();

private:
    Ui::SecuritySettingsWidget *ui;
};
