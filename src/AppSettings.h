// Vị trí: /src/appsettings.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Cung cấp quyền truy cập vào đối tượng QSettings toàn cục của ứng dụng.

#pragma once

class QSettings;

// Trả về một tham chiếu đến đối tượng QSettings duy nhất của ứng dụng.
// Đối tượng này trỏ đến file config.ini trong thư mục của file exe.
QSettings& getAppSettings();
