// Vị trí: /src/main.cpp
// Phiên bản: 2.1 (Thêm logic phục hồi CSDL khi khởi động)

#include "mainwindow.h"
#include "databasemanager.h"
#include "services/settingsservice.h"

#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <memory>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>

// Hàm kiểm tra và thực hiện phục hồi CSDL nếu được yêu cầu
void performRestoreIfRequested() {
    QSettings settings;
    QString backupPath = settings.value("Update/restoreDbOnStartup").toString();
    if (backupPath.isEmpty() || !QFile::exists(backupPath)) {
        settings.remove("Update/restoreDbOnStartup"); // Xóa cài đặt nếu không hợp lệ
        return;
    }

    QString currentDbPath = settings.value("databasePath").toString();
    if (currentDbPath.isEmpty()) {
        settings.remove("Update/restoreDbOnStartup");
        QMessageBox::critical(nullptr, "Lỗi Phục hồi", "Không tìm thấy đường dẫn CSDL hiện tại. Không thể phục hồi.");
        return;
    }

    // Cố gắng xóa file CSDL cũ trước
    if (QFile::exists(currentDbPath)) {
        if (!QFile::remove(currentDbPath)) {
            QMessageBox::critical(nullptr, "Lỗi Phục hồi", QString("Không thể xóa file CSDL cũ tại:\n%1\n\nVui lòng kiểm tra quyền hạn và thử lại.").arg(currentDbPath));
            settings.remove("Update/restoreDbOnStartup"); // Xóa cài đặt khi thất bại
            return;
        }
    }

    // Sao chép file backup để phục hồi
    if (QFile::copy(backupPath, currentDbPath)) {
        QMessageBox::information(nullptr, "Hoàn tất", "Đã phục hồi CSDL từ bản sao lưu thành công.");
    } else {
        QMessageBox::critical(nullptr, "Lỗi Phục hồi", QString("Không thể sao chép file sao lưu vào:\n%1\n\nVui lòng kiểm tra quyền hạn.").arg(currentDbPath));
    }

    // Xóa cài đặt phục hồi dù thành công hay thất bại
    settings.remove("Update/restoreDbOnStartup");
}

// Hàm lấy đường dẫn CSDL
QString getDatabasePath() {
    QSettings settings;
    QString dbPath = settings.value("databasePath").toString();

    if (dbPath.isEmpty() || !QFile::exists(dbPath)) {
        QMessageBox::information(nullptr, "Thiết lập Cơ sở dữ liệu",
                                 "Đây là lần đầu bạn sử dụng chương trình hoặc file CSDL không tìm thấy.\n"
                                 "Vui lòng chọn một vị trí để lưu file cơ sở dữ liệu (reuptool.db).");
        
        QString initialPath = QCoreApplication::applicationDirPath() + "/reuptool.db";
        dbPath = QFileDialog::getSaveFileName(nullptr, "Lưu Cơ sở dữ liệu", initialPath, "Database Files (*.db)");
        
        if (dbPath.isEmpty()) {
            return QString();
        }
        settings.setValue("databasePath", dbPath);
    }
    return dbPath;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("TMTSoftware");
    QCoreApplication::setApplicationName("ReupTool");

    QString configFilePath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QFileInfo(configFilePath).path());
    
    // Thực hiện phục hồi CSDL (nếu có yêu cầu) TRƯỚC KHI mở CSDL
    performRestoreIfRequested();

    // --- Áp dụng theme và font ngay khi khởi động ---
    {
        SettingsService settingsService;
        // Áp dụng Theme
        QString theme = settingsService.loadTheme();
        if (theme == "Dark") {
            a.setStyle(QStyleFactory::create("Fusion"));
            QPalette darkPalette;
            darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
            darkPalette.setColor(QPalette::WindowText, Qt::white);
            darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
            darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
            darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
            darkPalette.setColor(QPalette::ToolTipText, Qt::white);
            darkPalette.setColor(QPalette::Text, Qt::white);
            darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
            darkPalette.setColor(QPalette::ButtonText, Qt::white);
            darkPalette.setColor(QPalette::BrightText, Qt::red);
            darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
            darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
            darkPalette.setColor(QPalette::HighlightedText, Qt::black);
            a.setPalette(darkPalette);
        } else { // Light hoặc System
             a.setStyle(QStyleFactory::create("Fusion"));
             // Để palette mặc định
        }

        // Áp dụng Font
        a.setFont(settingsService.loadAppFont());
    }


    QString dbPath = getDatabasePath();
    if (dbPath.isEmpty()) {
        QMessageBox::critical(nullptr, "Lỗi", "Không có CSDL nào được chọn. Chương trình sẽ thoát.");
        return -1;
    }

    auto dbManager = std::make_unique<DatabaseManager>(dbPath);

    MainWindow w(std::move(dbManager));
    w.show();
    
    return a.exec();
}

