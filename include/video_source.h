#ifndef VIDEO_SOURCE_H
#define VIDEO_SOURCE_H

#include <opencv2/opencv.hpp>
#include <string>

class VideoSource
{
public:
    virtual ~VideoSource() = default;
    virtual bool open(const std::string &url) = 0;
    virtual bool isOpened() const = 0;
    virtual bool read(cv::Mat &frame) = 0;
    virtual void release() = 0;
};

class OpenCVVideoSource : public VideoSource
{
public:
    OpenCVVideoSource() = default;
    ~OpenCVVideoSource() override { release(); }

    bool open(const std::string &url) override
    {
        return cap_.open(url);
    }

    bool isOpened() const override
    {
        return cap_.isOpened();
    }

    bool read(cv::Mat &frame) override
    {
        return cap_.read(frame);
    }

    void release() override
    {
        if (cap_.isOpened())
        {
            cap_.release();
        }
    }

private:
    cv::VideoCapture cap_;
};

#endif // VIDEO_SOURCE_H
