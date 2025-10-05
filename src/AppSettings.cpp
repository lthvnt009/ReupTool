// Vị trí: /src/appsettings.cpp
// Phiên bản: 1.0 (Mới)

#include "appsettings.h"
#include <QSettings>
#include <QCoreApplication>
#include <QString>

QSettings& getAppSettings()
{
    // Sử dụng một đối tượng static để đảm bảo chỉ có một instance được tạo ra
    // và nó tồn tại trong suốt thời gian chạy của ứng dụng.
    static QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    return settings;
}
