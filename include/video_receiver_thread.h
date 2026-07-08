#ifndef VIDEO_RECEIVER_THREAD_H
#define VIDEO_RECEIVER_THREAD_H

#include "video_source.h"

#include <QThread>
#include <QString>
#include <QImage>
#include <memory>
#include <atomic>
#include <mutex>

class VideoReceiverThread : public QThread
{
    Q_OBJECT

public:
    explicit VideoReceiverThread(const QString &rtspUrl,
                                 std::unique_ptr<VideoSource> source = nullptr,
                                 QObject *parent = nullptr);
    ~VideoReceiverThread() override;

    void stop();
    void setRtspUrl(const QString &url);
    QString rtspUrl() const;
    QString currentStatus() const;

signals:
    void frameReceived(const QImage &image);
    void statusChanged(const QString &status);
    void errorOccurred(const QString &errorCode, const QString &message);

protected:
    void run() override;

private:
    QString rtspUrl_;
    std::unique_ptr<VideoSource> source_;
    std::atomic<bool> running_;
    QString currentStatus_;
    mutable std::mutex mutex_;
};

#endif // VIDEO_RECEIVER_THREAD_H
