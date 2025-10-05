// Vị trí: /src/services/fileimportservice.h
// Phiên bản: 1.4 (Thêm chức năng nhập link cho DataInput)
// Mô tả: Service chuyên trách đọc và phân tích dữ liệu từ các file.

#pragma once

#include <QObject>
#include <QList>
#include <QMap>
#include "../models.h"

class FileImportService : public QObject
{
    Q_OBJECT
public:
    explicit FileImportService(QObject *parent = nullptr);

    // Hàm nhập danh sách kênh từ file CSV.
    QList<Channel> importChannelsFromCsv(const QString& filePath);

    // Hàm nhập danh sách kênh từ file Excel XLSX.
    QList<Channel> importChannelsFromXlsx(const QString& filePath);
    
    // Hàm nhập danh sách video từ file XLSX (cho tab Thư viện).
    QList<Video> importVideosFromXlsx(const QString& filePath, const QMap<QString, int>& channelNameToIdMap);

    // Hàm nhập link video theo cấu trúc cho tab Nhập Dữ Liệu.
    QMap<QString, QStringList> importLinksForDataInput(const QString& filePath);


signals:
    void errorOccurred(const QString& errorMessage);

private:
    QMap<QString, QStringList> importFromTxt(const QString& filePath);
    QMap<QString, QStringList> importFromCsvOrXlsx(const QString& filePath);
};
