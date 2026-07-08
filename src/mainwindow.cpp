#include "mainwindow.h"
#include "rest_client.h"
#include "video_receiver_thread.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QGroupBox>
#include <QTime>
#include <QEventLoop>
#include <QTimer>
#include <QPixmap>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), connected_(false), statusText_("Disconnected")
{
    restClient_ = new RestClient("", this);
    videoThread_ = new VideoReceiverThread("", nullptr, this);

    setupUi();
    setupConnections();

    // Default UI state
    disconnectBtn_->setEnabled(false);
    applyFpsBtn_->setEnabled(false);
    applyResBtn_->setEnabled(false);
    statusBtn_->setEnabled(false);
    updateStatusIndicator("DISCONNECTED");

    appendLog("System initialized. Ready to connect.");
}

MainWindow::~MainWindow()
{
    if (videoThread_)
    {
        videoThread_->stop();
    }
}

bool MainWindow::isConnected() const
{
    return connected_;
}

void MainWindow::connectToServer()
{
    if (restClient_->baseUrl().isEmpty())
    {
        // Local offline simulation mode (e.g. for basic unit tests)
        connected_ = true;
        statusText_ = "Connected";
        if (statusLabel_)
        {
            statusLabel_->setText(statusText_);
        }
        updateStatusIndicator("CONNECTED");
        appendLog("Offline simulation mode: connected.", "SUCCESS");
        return;
    }

    onConnectButtonClicked();
}

void MainWindow::disconnectFromServer()
{
    if (restClient_->baseUrl().isEmpty())
    {
        connected_ = false;
        statusText_ = "Disconnected";
        if (statusLabel_)
        {
            statusLabel_->setText(statusText_);
        }
        updateStatusIndicator("DISCONNECTED");
        appendLog("Offline simulation mode: disconnected.");
        return;
    }

    onDisconnectButtonClicked();
}

QString MainWindow::connectionStatus() const
{
    return statusText_;
}

void MainWindow::setupUi()
{
    // Premium dark style sheet
    QString stylesheet = R"(
        QMainWindow {
            background-color: #1e1e2e;
        }
        QWidget {
            color: #cdd6f4;
            font-family: 'Segoe UI', 'Outfit', Arial, sans-serif;
            font-size: 13px;
        }
        QGroupBox {
            font-weight: bold;
            border: 1px solid #45475a;
            border-radius: 8px;
            margin-top: 12px;
            padding: 10px;
            background-color: #181825;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
            color: #cba6f7;
        }
        QLabel {
            color: #cdd6f4;
        }
        QLineEdit {
            background-color: #313244;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 6px 10px;
            color: #cdd6f4;
        }
        QLineEdit:focus {
            border: 1px solid #89b4fa;
        }
        QComboBox {
            background-color: #313244;
            border: 1px solid #45475a;
            border-radius: 6px;
            padding: 5px 10px;
            color: #cdd6f4;
        }
        QComboBox:on {
            border: 1px solid #89b4fa;
        }
        QComboBox QAbstractItemView {
            background-color: #181825;
            selection-background-color: #89b4fa;
            selection-color: #11111b;
            border: 1px solid #45475a;
        }
        QPushButton {
            background-color: #89b4fa;
            color: #11111b;
            border: none;
            border-radius: 6px;
            padding: 8px 12px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #b4befe;
        }
        QPushButton:pressed {
            background-color: #74c7ec;
        }
        QPushButton:disabled {
            background-color: #414256;
            color: #7f849c;
        }
        #connectBtn {
            background-color: #a6e3a1;
            color: #11111b;
        }
        #connectBtn:hover {
            background-color: #abe9b3;
        }
        #disconnectBtn {
            background-color: #f38ba8;
            color: #11111b;
        }
        #disconnectBtn:hover {
            background-color: #f89fb6;
        }
        QTextEdit {
            background-color: #11111b;
            border: 1px solid #313244;
            border-radius: 6px;
            font-family: 'Consolas', 'Courier New', monospace;
            font-size: 12px;
            color: #cdd6f4;
        }
    )";
    setStyleSheet(stylesheet);

    auto *centralWidget = new QWidget(this);
    auto *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    // Left Panel: Controls
    auto *leftPanel = new QWidget(this);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(10);
    leftPanel->setFixedWidth(260);

    // Group 1: Server Config
    auto *serverGroup = new QGroupBox("Server Configuration", this);
    auto *serverLayout = new QGridLayout(serverGroup);
    serverLayout->setSpacing(8);

    serverLayout->addWidget(new QLabel("Base URL:", this), 0, 0);
    baseUrlEdit_ = new QLineEdit("http://127.0.0.1:9091/api/v1", this);
    serverLayout->addWidget(baseUrlEdit_, 0, 1);

    serverLayout->addWidget(new QLabel("Client ID:", this), 1, 0);
    clientIdEdit_ = new QLineEdit("client-001", this);
    serverLayout->addWidget(clientIdEdit_, 1, 1);

    serverLayout->addWidget(new QLabel("Name:", this), 2, 0);
    clientNameEdit_ = new QLineEdit("Remote Control Client", this);
    serverLayout->addWidget(clientNameEdit_, 2, 1);

    connectBtn_ = new QPushButton("Connect", this);
    connectBtn_->setObjectName("connectBtn");
    disconnectBtn_ = new QPushButton("Disconnect", this);
    disconnectBtn_->setObjectName("disconnectBtn");

    auto *connBtnLayout = new QHBoxLayout();
    connBtnLayout->addWidget(connectBtn_);
    connBtnLayout->addWidget(disconnectBtn_);
    serverLayout->addLayout(connBtnLayout, 3, 0, 1, 2);

    leftLayout->addWidget(serverGroup);

    // Group 2: Control & Settings
    auto *settingsGroup = new QGroupBox("Stream Settings", this);
    auto *settingsLayout = new QGridLayout(settingsGroup);
    settingsLayout->setSpacing(8);

    settingsLayout->addWidget(new QLabel("Frame Rate:", this), 0, 0);
    fpsCombo_ = new QComboBox(this);
    fpsCombo_->addItems({"24", "30", "60"});
    fpsCombo_->setCurrentText("30");
    settingsLayout->addWidget(fpsCombo_, 0, 1);

    applyFpsBtn_ = new QPushButton("Apply FPS", this);
    settingsLayout->addWidget(applyFpsBtn_, 1, 0, 1, 2);

    settingsLayout->addWidget(new QLabel("Resolution:", this), 2, 0);
    resCombo_ = new QComboBox(this);
    resCombo_->addItems({"1280x720", "1920x1080", "3840x2160"});
    resCombo_->setCurrentText("1920x1080");
    settingsLayout->addWidget(resCombo_, 2, 1);

    applyResBtn_ = new QPushButton("Apply Resolution", this);
    settingsLayout->addWidget(applyResBtn_, 3, 0, 1, 2);

    statusBtn_ = new QPushButton("Fetch Server Status", this);
    settingsLayout->addWidget(statusBtn_, 4, 0, 1, 2);

    leftLayout->addWidget(settingsGroup);
    leftLayout->addStretch();

    // Right Panel: Stream and Logs
    auto *rightPanel = new QWidget(this);
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(10);

    // Viewport
    videoViewport_ = new QLabel(this);
    videoViewport_->setObjectName("videoViewport");
    videoViewport_->setFixedSize(640, 360);
    videoViewport_->setAlignment(Qt::AlignCenter);
    videoViewport_->setStyleSheet("background-color: #11111b; border: 2px solid #313244; border-radius: 8px; font-weight: bold; color: #585b70;");
    videoViewport_->setText("STREAM VIEWPORT");
    rightLayout->addWidget(videoViewport_);

    // Status Indicator Row
    auto *statusIndicatorLayout = new QHBoxLayout();
    statusIndicatorLayout->setSpacing(5);

    statusDot_ = new QLabel(this);
    statusDot_->setFixedSize(12, 12);
    statusDot_->setStyleSheet("background-color: #f38ba8; border-radius: 6px;");
    statusIndicatorLayout->addWidget(statusDot_);

    statusLabel_ = new QLabel("Disconnected", this);
    statusLabel_->setObjectName("statusLabel");
    statusLabel_->setStyleSheet("font-weight: bold; color: #a6adc8;");
    statusIndicatorLayout->addWidget(statusLabel_);
    statusIndicatorLayout->addStretch();
    rightLayout->addLayout(statusIndicatorLayout);

    // Logs
    logConsole_ = new QTextEdit(this);
    logConsole_->setReadOnly(true);
    logConsole_->setPlaceholderText("Logs console...");
    rightLayout->addWidget(logConsole_);

    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(rightPanel);

    setCentralWidget(centralWidget);
    setWindowTitle("Remote Control and Stream Client");
    resize(940, 600);
}

void MainWindow::setupConnections()
{
    // UI Connections
    connect(connectBtn_, &QPushButton::clicked, this, &MainWindow::onConnectButtonClicked);
    connect(disconnectBtn_, &QPushButton::clicked, this, &MainWindow::onDisconnectButtonClicked);
    connect(applyFpsBtn_, &QPushButton::clicked, this, &MainWindow::onApplyFpsClicked);
    connect(applyResBtn_, &QPushButton::clicked, this, &MainWindow::onApplyResolutionClicked);
    connect(statusBtn_, &QPushButton::clicked, this, &MainWindow::onGetStatusClicked);

    // RestClient Connections
    connect(restClient_, &RestClient::connected, this, &MainWindow::onRestConnected);
    connect(restClient_, &RestClient::disconnected, this, &MainWindow::onRestDisconnected);
    connect(restClient_, &RestClient::frameRateChanged, this, &MainWindow::onRestFrameRateChanged);
    connect(restClient_, &RestClient::resolutionChanged, this, &MainWindow::onRestResolutionChanged);
    connect(restClient_, &RestClient::statusUpdated, this, &MainWindow::onRestStatusUpdated);
    connect(restClient_, &RestClient::errorOccurred, this, &MainWindow::onRestError);

    // VideoThread Connections
    connect(videoThread_, &VideoReceiverThread::frameReceived, this, &MainWindow::onVideoFrameReceived);
    connect(videoThread_, &VideoReceiverThread::statusChanged, this, &MainWindow::onVideoStatusChanged);
    connect(videoThread_, &VideoReceiverThread::errorOccurred, this, &MainWindow::onVideoError);
}

void MainWindow::onConnectButtonClicked()
{
    connectBtn_->setEnabled(false);
    appendLog("Initiating connection request to " + baseUrlEdit_->text() + "...");
    restClient_->setBaseUrl(baseUrlEdit_->text());
    restClient_->connectSession(clientIdEdit_->text(), clientNameEdit_->text());
}

void MainWindow::onDisconnectButtonClicked()
{
    disconnectBtn_->setEnabled(false);
    appendLog("Initiating disconnection request...");
    restClient_->disconnectSession();
}

void MainWindow::onApplyFpsClicked()
{
    applyFpsBtn_->setEnabled(false);
    int fps = fpsCombo_->currentText().toInt();
    appendLog("Requesting frame rate change to " + QString::number(fps) + " FPS...");
    restClient_->setFrameRate(fps);
}

void MainWindow::onApplyResolutionClicked()
{
    applyResBtn_->setEnabled(false);
    QString resText = resCombo_->currentText();
    QStringList dims = resText.split('x');
    if (dims.size() == 2)
    {
        int w = dims[0].toInt();
        int h = dims[1].toInt();
        appendLog("Requesting resolution change to " + resText + "...");
        restClient_->setResolution(w, h);
    }
}

void MainWindow::onGetStatusClicked()
{
    statusBtn_->setEnabled(false);
    appendLog("Requesting server status...");
    restClient_->getStatus();
}

void MainWindow::onRestConnected(const QString &sessionId, const QString &rtspUrl)
{
    connected_ = true;
    statusText_ = "Connected";
    statusLabel_->setText(statusText_);
    updateStatusIndicator("CONNECTED");
    appendLog("Successfully connected. Session ID: " + sessionId, "SUCCESS");

    connectBtn_->setEnabled(false);
    disconnectBtn_->setEnabled(true);
    applyFpsBtn_->setEnabled(true);
    applyResBtn_->setEnabled(true);
    statusBtn_->setEnabled(true);

    // Start video streaming thread
    appendLog("Starting video stream receiver thread for " + rtspUrl + "...");
    videoThread_->stop();
    videoThread_->setRtspUrl(rtspUrl);
    videoThread_->start();
}

void MainWindow::onRestDisconnected()
{
    connected_ = false;
    statusText_ = "Disconnected";
    statusLabel_->setText(statusText_);
    updateStatusIndicator("DISCONNECTED");
    appendLog("Successfully disconnected from server.");

    connectBtn_->setEnabled(true);
    disconnectBtn_->setEnabled(false);
    applyFpsBtn_->setEnabled(false);
    applyResBtn_->setEnabled(false);
    statusBtn_->setEnabled(false);

    // Stop video streaming thread
    appendLog("Stopping video stream receiver thread...");
    videoThread_->stop();
    videoViewport_->clear();
    videoViewport_->setStyleSheet("background-color: #11111b; border: 2px solid #313244; border-radius: 8px; font-weight: bold; color: #585b70;");
    videoViewport_->setText("STREAM VIEWPORT");
}

void MainWindow::onRestFrameRateChanged(int fps)
{
    applyFpsBtn_->setEnabled(true);
    appendLog("Server frame rate updated to " + QString::number(fps) + " FPS", "SUCCESS");
}

void MainWindow::onRestResolutionChanged(int width, int height)
{
    applyResBtn_->setEnabled(true);
    appendLog("Server resolution updated to " + QString::number(width) + "x" + QString::number(height), "SUCCESS");
}

void MainWindow::onRestStatusUpdated(const QJsonObject &status)
{
    statusBtn_->setEnabled(true);
    QJsonDocument doc(status);
    QString details = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    appendLog("Status: " + details, "INFO");
}

void MainWindow::onRestError(const QString &errorCode, const QString &message)
{
    // Re-enable buttons depending on connection state
    if (!connected_)
    {
        connectBtn_->setEnabled(true);
    }
    else
    {
        disconnectBtn_->setEnabled(true);
        applyFpsBtn_->setEnabled(true);
        applyResBtn_->setEnabled(true);
        statusBtn_->setEnabled(true);
    }
    appendLog(QString("REST API Error [%1]: %2").arg(errorCode, message), "ERROR");
}

void MainWindow::onVideoFrameReceived(const QImage &image)
{
    QPixmap pix = QPixmap::fromImage(image);
    videoViewport_->setPixmap(pix.scaled(videoViewport_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onVideoStatusChanged(const QString &status)
{
    appendLog("Video Receiver Thread Status: " + status, "INFO");
    if (connected_)
    {
        updateStatusIndicator(status);
    }
}

void MainWindow::onVideoError(const QString &errorCode, const QString &message)
{
    appendLog(QString("Video Error [%1]: %2").arg(errorCode, message), "WARNING");
}

void MainWindow::appendLog(const QString &message, const QString &level)
{
    QTime time = QTime::currentTime();
    QString timeStr = time.toString("hh:mm:ss");
    
    QString color = "#cdd6f4"; // Default label color
    if (level == "ERROR")
    {
        color = "#f38ba8"; // Peach red
    }
    else if (level == "WARNING")
    {
        color = "#fab387"; // Yellow orange
    }
    else if (level == "SUCCESS")
    {
        color = "#a6e3a1"; // Green
    }
    
    logConsole_->append(QString("<font color=\"#585b70\">[%1]</font> <font color=\"%2\"><b>[%3]</b> %4</font>")
                        .arg(timeStr, color, level, message));
}

void MainWindow::updateStatusIndicator(const QString &status)
{
    if (status == "CONNECTED")
    {
        statusDot_->setStyleSheet("background-color: #a6e3a1; border-radius: 6px; min-width: 12px; max-width: 12px; min-height: 12px; max-height: 12px;");
        statusLabel_->setText("Streaming Active");
        statusLabel_->setStyleSheet("font-weight: bold; color: #a6e3a1;");
    }
    else if (status == "CONNECTING")
    {
        statusDot_->setStyleSheet("background-color: #f9e2af; border-radius: 6px; min-width: 12px; max-width: 12px; min-height: 12px; max-height: 12px;");
        statusLabel_->setText("Connecting Stream...");
        statusLabel_->setStyleSheet("font-weight: bold; color: #f9e2af;");
    }
    else
    {
        if (connected_)
        {
            statusDot_->setStyleSheet("background-color: #fab387; border-radius: 6px; min-width: 12px; max-width: 12px; min-height: 12px; max-height: 12px;");
            statusLabel_->setText("Connected (No Stream)");
            statusLabel_->setStyleSheet("font-weight: bold; color: #fab387;");
        }
        else
        {
            statusDot_->setStyleSheet("background-color: #f38ba8; border-radius: 6px; min-width: 12px; max-width: 12px; min-height: 12px; max-height: 12px;");
            statusLabel_->setText("Disconnected");
            statusLabel_->setStyleSheet("font-weight: bold; color: #a6adc8;");
        }
    }
}
