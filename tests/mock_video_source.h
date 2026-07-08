#ifndef MOCK_VIDEO_SOURCE_H
#define MOCK_VIDEO_SOURCE_H

#include "video_source.h"
#include <chrono>
#include <thread>
#include <atomic>

class MockVideoSource : public VideoSource
{
public:
    MockVideoSource()
        : openResult(true), isOpenedResult(true), readResult(true),
          openCallCount(0), readCallCount(0), releaseCallCount(0),
          delayMs(0), shouldFailOnReadCount(-1)
    {
        // Default frame: a simple 100x100 white image
        defaultFrame = cv::Mat(100, 100, CV_8UC3, cv::Scalar(255, 255, 255));
    }

    bool open(const std::string &url) override
    {
        openCallCount++;
        urlOpened = url;
        return openResult;
    }

    bool isOpened() const override
    {
        return isOpenedResult;
    }

    bool read(cv::Mat &frame) override
    {
        readCallCount++;
        if (delayMs > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }

        if (shouldFailOnReadCount > 0 && readCallCount >= shouldFailOnReadCount)
        {
            return false;
        }

        if (!readResult)
        {
            return false;
        }

        defaultFrame.copyTo(frame);
        return true;
    }

    void release() override
    {
        releaseCallCount++;
        isOpenedResult = false;
    }

    // Controls for tests
    std::atomic<bool> openResult;
    std::atomic<bool> isOpenedResult;
    std::atomic<bool> readResult;
    std::atomic<int> openCallCount;
    std::atomic<int> readCallCount;
    std::atomic<int> releaseCallCount;
    std::atomic<int> delayMs;
    std::atomic<int> shouldFailOnReadCount;
    std::string urlOpened;
    cv::Mat defaultFrame;
};

#endif // MOCK_VIDEO_SOURCE_H
