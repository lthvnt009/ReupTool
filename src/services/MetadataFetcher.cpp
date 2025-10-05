// Vị trí: /src/services/metadatafetcher.cpp
// Phiên bản: 1.0 (Mới)

#include "metadatafetcher.h"
#include "../widgets/channeldatawidget.h"
#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

MetadataFetcher::MetadataFetcher(QObject *parent)
    : QObject(parent),
      m_process(new QProcess(this)),
      m_isBusy(false)
{
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MetadataFetcher::onProcessFinished);
}

MetadataFetcher::~MetadataFetcher()
{
    clearQueueAndStop();
}

void MetadataFetcher::fetch(ChannelDataWidget* requester)
{
    if (!requester) return;
    m_queue.enqueue(requester);
    startNextFetch();
}

void MetadataFetcher::clearQueueAndStop()
{
    m_queue.clear();
    if (m_process->state() == QProcess::Running) {
        // Ngắt kết nối để onProcessFinished không được gọi khi ta chủ động kill
        disconnect(m_process, &QProcess::finished, this, &MetadataFetcher::onProcessFinished);
        m_process->kill();
        // Kết nối lại cho các lần chạy sau
        connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &MetadataFetcher::onProcessFinished);
    }
    m_isBusy = false;
}

void MetadataFetcher::startNextFetch()
{
    if (m_isBusy || m_queue.isEmpty()) {
        return;
    }
    m_isBusy = true;

    ChannelDataWidget* currentWidget = m_queue.head(); // Chỉ xem, chưa xóa
    m_process->setProperty("requesterWidget", QVariant::fromValue<QObject*>(currentWidget));

    QString ytdlpPath = QCoreApplication::applicationDirPath() + "/lib/yt-dlp.exe";
    QStringList arguments;
    arguments << "--dump-json" << "-q" << "--no-warnings" << currentWidget->getVideoData().videoUrl;
    m_process->start(ytdlpPath, arguments);
}

void MetadataFetcher::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ChannelDataWidget* requester = qobject_cast<ChannelDataWidget*>(m_process->property("requesterWidget").value<QObject*>());
    
    if (!m_queue.isEmpty()) {
        m_queue.dequeue();
    }
    
    if (!requester || exitStatus == QProcess::CrashExit) {
        if (requester) {
            emit fetchFailed(requester, "Đã hủy");
        }
        m_isBusy = false;
        startNextFetch();
        return;
    }

    QString output = m_process->readAllStandardOutput();
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    QJsonObject obj = doc.object();

    if (exitCode == 0 && !obj.isEmpty()) {
        QString title = obj["title"].toString();
        QString description = obj["description"].toString();
        emit metadataReady(requester, title, description);
    } else {
        emit fetchFailed(requester, "Lỗi lấy thông tin");
    }

    m_isBusy = false;
    startNextFetch();
}

