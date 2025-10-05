// Phiên bản: 1.3 (Sửa lỗi build)
// Vị trí: /src/services/downloadtask.h
// Mô tả: Lớp thực thi một tác vụ tải về duy nhất trên một luồng riêng.

#pragma once

#include <QObject>
#include <QRunnable>
#include <QProcess>
#include "../models.h"

class DownloadTask : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit DownloadTask(const DownloadItem& item);
    void run() override;
    void stop();
    QString uuid() const; // *** SỬA LỖI BUILD: Thêm hàm getter ***

signals:
    void progressUpdated(const QString& uuid, int percentage, qint64 size, const QString& speed, const QString& eta);
    void finished(const QString& uuid, bool success, const QString& finalPath);
    void errorOccurred(const QString& uuid, const QString& errorMessage);
    void titleResolved(const QString& uuid, const QString& title);
    void rawOutputReady(const QString& uuid, const QString& output);

private slots:
    void onProcessOutput();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QStringList buildArguments();

    DownloadItem m_item;
    QProcess* m_process = nullptr;
    bool m_isStopped = false;
    QString m_finalFilePath;
};

