// Phiên bản: 1.7 (Sử dụng AppSettings)
// Vị trí: /src/services/downloadtask.cpp

#include "downloadtask.h"
#include "../appsettings.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>
#include <QRegularExpression>
#include <QSettings>

DownloadTask::DownloadTask(const DownloadItem& item)
    : QObject(nullptr) // QRunnable không có parent, phải tự quản lý
    , m_item(item)
{
}

void DownloadTask::run()
{
    m_process = new QProcess();

    connect(m_process, &QProcess::readyReadStandardOutput, this, &DownloadTask::onProcessOutput);
    connect(m_process, &QProcess::finished, this, &DownloadTask::onProcessFinished);

    QStringList arguments = buildArguments();
    
    QSettings& settings = getAppSettings();
    QString ytdlpPath = settings.value("DownloadSettings/ytdlpPath", "lib/yt-dlp.exe").toString();

    m_process->start(ytdlpPath, arguments);
    m_process->waitForFinished(-1); // Chờ vô hạn cho đến khi process kết thúc

    delete m_process;
    m_process = nullptr;
}

void DownloadTask::stop()
{
    m_isStopped = true;
    if (m_process && m_process->state() == QProcess::Running) {
        m_process->kill();
    }
}

QString DownloadTask::uuid() const
{
    return m_item.uuid;
}


QStringList DownloadTask::buildArguments()
{
    QStringList args;
    const DownloadOptions& opts = m_item.options;

    // Output template
    QString finalTemplate = opts.outputTemplate;
    // Đảm bảo đường dẫn lưu file được áp dụng
    if(!opts.savePath.isEmpty()){
        finalTemplate = QDir(opts.savePath).filePath(finalTemplate);
    }
    args << "-o" << finalTemplate;

    // Format and Quality
    QString formatSelector;
    if (opts.downloadType == "Video") {
        if(opts.quality != "Best") {
            QString qualityValue = opts.quality;
            formatSelector = QString("bestvideo[height<=%1][ext=%2]+bestaudio/best[height<=%1][ext=%2]/best")
                .arg(qualityValue.replace("p", ""))
                .arg(opts.format.toLower());
        } else {
             formatSelector = QString("bestvideo[ext=%1]+bestaudio/best[ext=%1]/best").arg(opts.format.toLower());
        }
    } else { // Audio
        args << "-x"; // --extract-audio
        args << "--audio-format" << opts.format.toLower();
        if(opts.quality == "cao") args << "--audio-quality" << "0";
        else if(opts.quality == "trung bình") args << "--audio-quality" << "5";
        else if(opts.quality == "thấp") args << "--audio-quality" << "9";
    }
    if(!formatSelector.isEmpty()) args << "-f" << formatSelector;


    // Thumbnail
    if (opts.thumbnailFormat != "Bỏ qua") {
        args << "--write-thumbnail";
        if (opts.thumbnailFormat != "jpg") { // jpg là mặc định
            args << "--convert-thumbnails" << opts.thumbnailFormat.toLower();
        }
    }

    // Subtitles
    if (opts.subtitleLang != "Bỏ qua") {
        args << "--write-subs";
        if(opts.subtitleLang == "Tiếng anh"){
            args << "--sub-langs" << "en.*,-live_chat";
        } else { // Tự động
             args << "--sub-langs" << "vi.*,-live_chat";
        }
    }

    // Write description
    if(opts.writeDescription){
        args << "--write-description";
    }

    // Cut Video
    if (opts.cutVideo && !opts.cutTimeRange.isEmpty()) {
        args << "--download-sections" << QString("*%1").arg(opts.cutTimeRange);
    }

    // SponsorBlock
    if(!opts.sponsorBlockRemove.isEmpty()){
        args << "--sponsorblock-remove" << opts.sponsorBlockRemove.join(",");
    }
    if(!opts.sponsorBlockMark.isEmpty()){
         args << "--sponsorblock-mark" << opts.sponsorBlockMark.join(",");
    }

    // Custom Command
    if (opts.useCustomCommand && !opts.customCommand.isEmpty()) {
        args.append(opts.customCommand.split(' '));
    }
    
    // FFMPEG path
    QSettings& settings = getAppSettings();
    QString ffmpegPath = settings.value("DownloadSettings/ffmpegPath", "lib/ffmpeg/bin").toString();
    if(!ffmpegPath.isEmpty()) {
        args << "--ffmpeg-location" << ffmpegPath;
    }


    // URL cuối cùng
    args << m_item.url;

    return args;
}


void DownloadTask::onProcessOutput()
{
    if(!m_process) return;

    QString output = m_process->readAllStandardOutput();
    emit rawOutputReady(m_item.uuid, output);

    // Regex for download progress
    QRegularExpression progressRe("\\[download\\]\\s+([\\d\\.]+)% of.*ETA (\\S+).*at (\\S+)");
    QRegularExpressionMatch progressMatch = progressRe.match(output);
    if (progressMatch.hasMatch()) {
        int percentage = progressMatch.captured(1).toDouble();
        QString sizeStr; // Kích thước tổng sẽ được xử lý sau nếu cần
        QString speed = progressMatch.captured(3);
        QString eta = progressMatch.captured(2);
        emit progressUpdated(m_item.uuid, percentage, 0, speed, eta);
    }
    
    // Regex for destination file (khi tải xong)
    QRegularExpression destRe("\\[download\\] Destination: (.*)");
    QRegularExpressionMatch destMatch = destRe.match(output);
    if(destMatch.hasMatch()){
        m_finalFilePath = destMatch.captured(1).trimmed();
    }
    
    // Regex for merging (sau khi tải xong)
    QRegularExpression mergeRe("\\[Merger\\] Merging formats into \"(.*)\"");
     QRegularExpressionMatch mergeMatch = mergeRe.match(output);
    if(mergeMatch.hasMatch()){
         m_finalFilePath = mergeMatch.captured(1).trimmed();
    }
    
     // Regex for getting title
    QRegularExpression titleRe("\\[info\\]\\s+(.*): Downloading webpage");
    QRegularExpressionMatch titleMatch = titleRe.match(output);
    if(titleMatch.hasMatch()){
        emit titleResolved(m_item.uuid, titleMatch.captured(1).trimmed());
    }
}

void DownloadTask::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_isStopped) {
         emit finished(m_item.uuid, false, "");
        return;
    }

    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
        emit finished(m_item.uuid, true, m_finalFilePath);
    } else {
        QString errorLog = m_process->readAllStandardError();
        if(errorLog.isEmpty()){
            errorLog = "Lỗi không xác định. Vui lòng kiểm tra Log chi tiết.";
        }
        emit errorOccurred(m_item.uuid, errorLog);
    }
}
