// Vị trí: src/widgets/settings/AppearanceSettingsWidget.h
// Phiên bản: 1.1 (Hoàn thiện giao diện và tín hiệu)

#pragma once

#include <QWidget>
#include <QFont>

namespace Ui { class AppearanceSettingsWidget; }

class QButtonGroup;

class AppearanceSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AppearanceSettingsWidget(QWidget *parent = nullptr);
    ~AppearanceSettingsWidget();

    void setTheme(const QString& theme);
    void setFont(const QFont& font);

signals:
    void themeChanged(const QString& theme);
    void fontChangeRequested();

private:
    Ui::AppearanceSettingsWidget *ui;
    QButtonGroup* m_themeGroup;
};

