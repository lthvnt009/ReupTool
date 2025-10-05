// Vị trí: src/widgets/settings/GeneralSettingsWidget.h
// Phiên bản: 1.1 (Hoàn thiện giao diện và tín hiệu)

#pragma once

#include <QWidget>

namespace Ui { class GeneralSettingsWidget; }

class GeneralSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GeneralSettingsWidget(QWidget *parent = nullptr);
    ~GeneralSettingsWidget();

    void setLanguage(const QString& language);
    void setRememberWindowState(bool enabled);

signals:
    void languageChanged(const QString& language);
    void rememberWindowStateChanged(bool enabled);

private:
    Ui::GeneralSettingsWidget *ui;
};

