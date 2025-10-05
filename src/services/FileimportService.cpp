// Vị trí: /src/services/fileimportservice.cpp
// Phiên bản: 1.4 (Thêm chức năng nhập link cho DataInput)
// Mô tả: Triển khai logic đọc file CSV và XLSX cho FileImportService.

#include "fileimportservice.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

// Include header của thư viện QXlsx
#include "xlsxdocument.h"

FileImportService::FileImportService(QObject *parent)
    : QObject{parent}
{
}

QList<Channel> FileImportService::importChannelsFromCsv(const QString &filePath)
{
    QList<Channel> importedChannels;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(QString("Không thể mở file CSV: %1").arg(file.errorString()));
        return importedChannels;
    }

    QTextStream in(&file);
    int lineNumber = 0;
    while (!in.atEnd()) {
        lineNumber++;
        QString line = in.readLine();
        QStringList fields = line.split(',');

        if (fields.size() >= 2) {
            QString name = fields.at(0).trimmed();
            QString link = fields.at(1).trimmed();

            if (!name.isEmpty() && !link.isEmpty()) {
                Channel ch;
                ch.name = name;
                ch.link = link;
                importedChannels.append(ch);
            } else {
                 qWarning() << "Bỏ qua dòng" << lineNumber << "do thiếu tên hoặc link.";
            }
        } else {
            qWarning() << "Bỏ qua dòng" << lineNumber << "do không đủ 2 cột.";
        }
    }

    file.close();
    return importedChannels;
}

QList<Channel> FileImportService::importChannelsFromXlsx(const QString &filePath)
{
    QList<Channel> importedChannels;
    
    QXlsx::Document xlsx(filePath);
    
    if (!xlsx.load()) {
        emit errorOccurred("Không thể mở hoặc đọc file Excel.");
        return importedChannels;
    }

    QXlsx::Worksheet *sheet = xlsx.currentWorksheet();
    if (!sheet) {
        emit errorOccurred("File Excel không có sheet nào.");
        return importedChannels;
    }

    int maxRow = sheet->dimension().lastRow();
    
    for (int row = 1; row <= maxRow; ++row) {
        QVariant nameVariant = sheet->read(row, 1);
        QVariant linkVariant = sheet->read(row, 2);

        QString name = nameVariant.toString().trimmed();
        QString link = linkVariant.toString().trimmed();

        if (!name.isEmpty() && !link.isEmpty()) {
            Channel ch;
            ch.name = name;
            ch.link = link;
            importedChannels.append(ch);
        } else {
            qWarning() << "Bỏ qua hàng" << row << "trong file Excel do thiếu tên hoặc link.";
        }
    }

    return importedChannels;
}

QList<Video> FileImportService::importVideosFromXlsx(const QString& filePath, const QMap<QString, int>& channelNameToIdMap)
{
    QList<Video> importedVideos;
    QXlsx::Document xlsx(filePath);

    if (!xlsx.load()) {
        emit errorOccurred("Không thể mở hoặc đọc file Excel.");
        return importedVideos;
    }

    QXlsx::Worksheet *sheet = xlsx.currentWorksheet();
    if (!sheet) {
        emit errorOccurred("File Excel không có sheet nào.");
        return importedVideos;
    }

    int maxRow = sheet->dimension().lastRow();
    if (maxRow <= 1) return importedVideos; // Bỏ qua nếu chỉ có header

    // Đọc header để xác định vị trí các cột
    QMap<QString, int> headerMap;
    for (int col = 1; col <= sheet->dimension().lastColumn(); ++col) {
        headerMap[sheet->read(1, col).toString().trimmed()] = col;
    }

    // Kiểm tra các cột bắt buộc
    if (!headerMap.contains("Kênh") || !headerMap.contains("Link Video")) {
        emit errorOccurred("File Excel phải chứa ít nhất cột 'Kênh' và 'Link Video'.");
        return importedVideos;
    }

    for (int row = 2; row <= maxRow; ++row) {
        QString channelName = sheet->read(row, headerMap["Kênh"]).toString().trimmed();
        QString videoUrl = sheet->read(row, headerMap["Link Video"]).toString().trimmed();

        if (channelName.isEmpty() || videoUrl.isEmpty() || !channelNameToIdMap.contains(channelName)) {
            qWarning() << "Bỏ qua dòng" << row << "do thiếu thông tin, sai tên kênh, hoặc link không hợp lệ.";
            continue;
        }

        Video v;
        v.channelId = channelNameToIdMap.value(channelName);
        v.videoUrl = videoUrl;
        v.newTitle = sheet->read(row, headerMap.value("Tiêu đề mới", 0)).toString();
        v.newDescription = sheet->read(row, headerMap.value("Mô tả mới", 0)).toString();
        v.newTags = sheet->read(row, headerMap.value("Tags mới", 0)).toString();
        v.newSubTags = sheet->read(row, headerMap.value("Tags con mới", 0)).toString();
        v.status = sheet->read(row, headerMap.value("Trạng thái", 0)).toString();
        v.videoDate = QDate::fromString(sheet->read(row, headerMap.value("Ngày video", 0)).toString(), Qt::ISODate);

        importedVideos.append(v);
    }

    return importedVideos;
}

QMap<QString, QStringList> FileImportService::importLinksForDataInput(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "txt") {
        return importFromTxt(filePath);
    } else if (suffix == "csv" || suffix == "xlsx" || suffix == "xls") {
        return importFromCsvOrXlsx(filePath);
    }

    emit errorOccurred("Định dạng file không được hỗ trợ. Vui lòng chọn file .txt, .csv, hoặc .xlsx.");
    return {};
}

QMap<QString, QStringList> FileImportService::importFromTxt(const QString &filePath)
{
    QMap<QString, QStringList> channelUrlMap;
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(QString("Không thể mở file: %1").arg(file.errorString()));
        return channelUrlMap;
    }

    QTextStream in(&file);
    QString currentChannel;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        // Giả định đơn giản: nếu không bắt đầu bằng http, đó là tên kênh
        if (line.startsWith("http", Qt::CaseInsensitive)) {
            if (!currentChannel.isEmpty()) {
                channelUrlMap[currentChannel].append(line);
            }
        } else {
            currentChannel = line;
            if (!channelUrlMap.contains(currentChannel)) {
                channelUrlMap[currentChannel] = QStringList();
            }
        }
    }
    file.close();
    return channelUrlMap;
}

QMap<QString, QStringList> FileImportService::importFromCsvOrXlsx(const QString &filePath)
{
    QMap<QString, QStringList> channelUrlMap;
    // Thư viện QXlsx có thể đọc cả file .csv đơn giản
    QXlsx::Document doc(filePath);
    
    if (!doc.load()) {
        emit errorOccurred("Không thể mở hoặc đọc file Excel/CSV.");
        return channelUrlMap;
    }

    QXlsx::Worksheet *sheet = doc.currentWorksheet();
    if (!sheet) {
        emit errorOccurred("File không có sheet nào hợp lệ.");
        return channelUrlMap;
    }

    QXlsx::CellRange dimension = sheet->dimension();
    int maxCol = dimension.lastColumn();
    int maxRow = dimension.lastRow();

    for (int col = 1; col <= maxCol; ++col) {
        QString channelName = sheet->read(1, col).toString().trimmed();
        if (channelName.isEmpty()) continue;

        QStringList urls;
        for (int row = 2; row <= maxRow; ++row) {
            QString url = sheet->read(row, col).toString().trimmed();
            // Chỉ thêm các dòng trông giống URL
            if (url.startsWith("http", Qt::CaseInsensitive)) {
                urls.append(url);
            }
        }

        if (!urls.isEmpty()) {
            // Ghi đè hoặc thêm mới danh sách link cho kênh
            channelUrlMap[channelName] = urls;
        }
    }
    return channelUrlMap;
}
