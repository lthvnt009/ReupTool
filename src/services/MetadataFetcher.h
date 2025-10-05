// Vị trí: /src/services/metadatafetcher.h
// Phiên bản: 1.0 (Mới)
// Mô tả: Lớp chuyên trách xử lý việc lấy metadata video một cách tuần tự.

#pragma once

#include <QObject>
#include <QProcess>
#include <QQueue>
#include <QVariant>

// Forward declaration
class ChannelDataWidget;

class MetadataFetcher : public QObject
{
    Q_OBJECT
public:
    explicit MetadataFetcher(QObject *parent = nullptr);
    ~MetadataFetcher();

    void fetch(ChannelDataWidget* requester);
    void clearQueueAndStop();

signals:
    void metadataReady(ChannelDataWidget* requester, const QString& title, const QString& description);
    void fetchFailed(ChannelDataWidget* requester, const QString& errorMessage);

private slots:
    void startNextFetch();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess* m_process;
    QQueue<ChannelDataWidget*> m_queue;
    bool m_isBusy;
};

