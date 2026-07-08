#include "video_receiver_thread.h"

#include <QDebug>
#include <QElapsedTimer>
#include <chrono>
#include <thread>

VideoReceiverThread::VideoReceiverThread(const QString &rtspUrl,
                                         std::unique_ptr<VideoSource> source,
                                         QObject *parent)
    : QThread(parent), rtspUrl_(rtspUrl), source_(std::move(source)), running_(false), currentStatus_("DISCONNECTED")
{
    if (!source_)
    {
        source_ = std::make_unique<OpenCVVideoSource>();
    }
}

VideoReceiverThread::~VideoReceiverThread()
{
    stop();
}

void VideoReceiverThread::stop()
{
    running_ = false;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (source_)
        {
            source_->release();
        }
    }
    wait();
}

void VideoReceiverThread::setRtspUrl(const QString &url)
{
    std::lock_guard<std::mutex> lock(mutex_);
    rtspUrl_ = url;
}

QString VideoReceiverThread::rtspUrl() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return rtspUrl_;
}

QString VideoReceiverThread::currentStatus() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return currentStatus_;
}

void VideoReceiverThread::run()
{
    running_ = true;
    double backoffSeconds = 3.0;
    const double maxBackoff = 60.0;
    const double backoffMultiplier = 1.5;

    while (running_)
    {
        QString url;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            url = rtspUrl_;
            currentStatus_ = "CONNECTING";
            emit statusChanged(currentStatus_);
        }

        qDebug() << "VideoReceiverThread: Attempting to connect to" << url;

        bool opened = false;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (source_)
            {
                opened = source_->open(url.toStdString());
            }
        }

        if (!opened)
        {
            {
                std::lock_guard<std::mutex> lock(mutex_);
                currentStatus_ = "DISCONNECTED";
                emit statusChanged(currentStatus_);
            }
            emit errorOccurred("RTSP_CONNECTION_FAILED", "Cannot establish RTSP connection");

            // Sleep with exponential backoff (checking running_ state)
            qDebug() << "VideoReceiverThread: Connection failed. Backing off for" << backoffSeconds << "seconds.";
            int sleepSteps = static_cast<int>(backoffSeconds * 10);
            for (int i = 0; i < sleepSteps && running_; ++i)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            backoffSeconds = std::min(backoffSeconds * backoffMultiplier, maxBackoff);
            continue;
        }

        // Successfully connected
        {
            std::lock_guard<std::mutex> lock(mutex_);
            currentStatus_ = "CONNECTED";
            emit statusChanged(currentStatus_);
        }
        backoffSeconds = 3.0; // Reset backoff

        qDebug() << "VideoReceiverThread: Connected to stream successfully.";

        QElapsedTimer lastFrameTimer;
        lastFrameTimer.start();

        while (running_)
        {
            bool isOpened = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (source_)
                {
                    isOpened = source_->isOpened();
                }
            }

            if (!isOpened)
            {
                emit errorOccurred("STREAM_INTERRUPTION", "Video stream interrupted");
                break;
            }

            cv::Mat frame;
            bool success = false;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (source_)
                {
                    success = source_->read(frame);
                }
            }

            if (!success)
            {
                emit errorOccurred("STREAM_INTERRUPTION", "Video stream interrupted");
                break;
            }

            if (!frame.empty())
            {
                lastFrameTimer.restart();

                // Convert BGR to RGB
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

                // Convert to QImage
                QImage qimg((const unsigned char *)frame.data,
                            frame.cols,
                            frame.rows,
                            static_cast<int>(frame.step),
                            QImage::Format_RGB888);

                emit frameReceived(qimg.copy()); // Deep copy to prevent issues in UI thread
            }
            else
            {
                // Frame is empty but read succeeded (could be transient)
                if (lastFrameTimer.hasExpired(3000))
                {
                    emit errorOccurred("FRAME_TIMEOUT", "Frame reception timeout");
                    break;
                }
                
                // Sleep briefly to avoid busy wait if reading is non-blocking
                std::this_thread::sleep_for(std::chrono::milliseconds(30));
            }

            // Check frame timeout
            if (lastFrameTimer.hasExpired(3000))
            {
                emit errorOccurred("FRAME_TIMEOUT", "Frame reception timeout");
                break;
            }
        }

        // Cleanup before reconnecting or exiting
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (source_)
            {
                source_->release();
            }
            currentStatus_ = "DISCONNECTED";
            emit statusChanged(currentStatus_);
        }

        if (running_)
        {
            // Pause before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }
}
