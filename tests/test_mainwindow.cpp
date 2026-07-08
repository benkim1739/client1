#include "mainwindow.h"
#include "rest_client.h"
#include "video_receiver_thread.h"
#include "mock_video_source.h"
#include "mock_http_server.h"

#include <QApplication>
#include <QEventLoop>
#include <QTimer>
#include <gtest/gtest.h>
#include <memory>

namespace
{
    QApplication *ensureApplication()
    {
        static int argc = 1;
        static char *argv[] = {(char *)"client1_tests", nullptr};
        static QApplication app(argc, argv);
        return &app;
    }
} // namespace

// 1. Existing Test: MainWindow UI simulation fallback
TEST(MainWindowTest, ConnectAndDisconnectUpdatesState)
{
    ensureApplication();
    MainWindow window;

    // By default, baseUrl is empty, so it uses offline simulation
    window.connectToServer();
    EXPECT_TRUE(window.isConnected());
    EXPECT_EQ(window.connectionStatus(), QString("Connected"));

    window.disconnectFromServer();
    EXPECT_FALSE(window.isConnected());
    EXPECT_EQ(window.connectionStatus(), QString("Disconnected"));
}

// 2. RestClient Tests using MockHttpServer
TEST(RestClientTest, FullSessionAndCommandsFlow)
{
    ensureApplication();

    // Setup local mock HTTP server on a random free port
    MockHttpServer server;
    ASSERT_TRUE(server.listen(QHostAddress::LocalHost, 0));
    QString baseUrl = QString("http://127.0.0.1:%1/api/v1").arg(server.serverPort());

    RestClient client(baseUrl);
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    // Test Connect
    bool connectedEmitted = false;
    QString rcvdSessionId;
    QString rcvdRtspUrl;

    QObject::connect(&client, &RestClient::connected, &loop, [&](const QString &sid, const QString &url) {
        connectedEmitted = true;
        rcvdSessionId = sid;
        rcvdRtspUrl = url;
        loop.quit();
    });
    QObject::connect(&client, &RestClient::errorOccurred, &loop, [&](const QString &err, const QString &msg) {
        ADD_FAILURE() << "Error occurred during connect: " << err.toStdString() << " - " << msg.toStdString();
        loop.quit();
    });
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timeoutTimer.start(2000);
    client.connectSession("test-client-id", "Test Client");
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(connectedEmitted);
    EXPECT_EQ(rcvdSessionId, QString("sess-abc123"));
    EXPECT_EQ(rcvdRtspUrl, QString("rtsp://127.0.0.1:8554/camera"));
    EXPECT_TRUE(client.isConnected());
    EXPECT_EQ(client.sessionId(), QString("sess-abc123"));

    // Test Get Status
    bool statusEmitted = false;
    QJsonObject rcvdStatus;
    QObject::disconnect(&client, nullptr, nullptr, nullptr); // Disconnect all slots
    QObject::connect(&client, &RestClient::statusUpdated, &loop, [&](const QJsonObject &status) {
        statusEmitted = true;
        rcvdStatus = status;
        loop.quit();
    });
    QObject::connect(&client, &RestClient::errorOccurred, &loop, [&](const QString &err, const QString &msg) {
        ADD_FAILURE() << "Error occurred during status check: " << err.toStdString() << " - " << msg.toStdString();
        loop.quit();
    });

    timeoutTimer.start(2000);
    client.getStatus();
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(statusEmitted);
    EXPECT_EQ(rcvdStatus.value("connection_status").toString(), QString("CONNECTED"));
    EXPECT_EQ(rcvdStatus.value("current_fps").toInt(), 30);

    // Test Set Frame Rate
    bool fpsEmitted = false;
    int rcvdFps = 0;
    QObject::disconnect(&client, nullptr, nullptr, nullptr);
    QObject::connect(&client, &RestClient::frameRateChanged, &loop, [&](int fps) {
        fpsEmitted = true;
        rcvdFps = fps;
        loop.quit();
    });

    timeoutTimer.start(2000);
    client.setFrameRate(60);
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(fpsEmitted);
    EXPECT_EQ(rcvdFps, 60);

    // Test Set Resolution
    bool resEmitted = false;
    int rcvdWidth = 0, rcvdHeight = 0;
    QObject::disconnect(&client, nullptr, nullptr, nullptr);
    QObject::connect(&client, &RestClient::resolutionChanged, &loop, [&](int w, int h) {
        resEmitted = true;
        rcvdWidth = w;
        rcvdHeight = h;
        loop.quit();
    });

    timeoutTimer.start(2000);
    client.setResolution(1280, 720);
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(resEmitted);
    EXPECT_EQ(rcvdWidth, 1280);
    EXPECT_EQ(rcvdHeight, 720);

    // Test Disconnect
    bool disconnectedEmitted = false;
    QObject::disconnect(&client, nullptr, nullptr, nullptr);
    QObject::connect(&client, &RestClient::disconnected, &loop, [&]() {
        disconnectedEmitted = true;
        loop.quit();
    });

    timeoutTimer.start(2000);
    client.disconnectSession();
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(disconnectedEmitted);
    EXPECT_FALSE(client.isConnected());
}

// 3. VideoReceiverThread Tests using MockVideoSource
TEST(VideoReceiverThreadTest, StreamCaptureAndErrorHandling)
{
    ensureApplication();

    auto mockSource = std::make_unique<MockVideoSource>();
    MockVideoSource *mockPtr = mockSource.get(); // Keep a raw pointer to control mock state

    VideoReceiverThread thread("rtsp://127.0.0.1:8554/camera", std::move(mockSource));
    QEventLoop loop;
    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);

    // Test success frame reception
    bool frameReceived = false;
    QObject::connect(&thread, &VideoReceiverThread::frameReceived, &loop, [&](const QImage &img) {
        frameReceived = true;
        loop.quit();
    });
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);

    thread.start();
    timeoutTimer.start(2000);
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(frameReceived);
    EXPECT_EQ(thread.currentStatus(), QString("CONNECTED"));
    EXPECT_GT(mockPtr->readCallCount.load(), 0);

    // Test stream interruption handling
    bool errorEmitted = false;
    QString rcvdErrCode;
    QObject::disconnect(&thread, nullptr, nullptr, nullptr);
    QObject::connect(&thread, &VideoReceiverThread::errorOccurred, &loop, [&](const QString &code, const QString &msg) {
        errorEmitted = true;
        rcvdErrCode = code;
        loop.quit();
    });

    // Cause subsequent reads to fail
    mockPtr->readResult = false;
    
    timeoutTimer.start(2000);
    loop.exec();
    timeoutTimer.stop();

    EXPECT_TRUE(errorEmitted);
    EXPECT_TRUE(rcvdErrCode == "STREAM_INTERRUPTION" || rcvdErrCode == "FRAME_TIMEOUT");

    thread.stop();
}
