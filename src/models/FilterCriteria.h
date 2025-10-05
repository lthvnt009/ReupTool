// Vị trí: /src/models/FilterCriteria.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Định nghĩa cấu trúc dữ liệu để truyền các tiêu chí lọc và tìm kiếm.

#pragma once

#include <QString>

struct FilterCriteria
{
    int page = 1;
    int pageSize = 50; // Mặc định hiển thị 50 mục mỗi trang
    int channelId = -1; // -1 nghĩa là tất cả các kênh
    QString status;
    QString searchTerm;
};
