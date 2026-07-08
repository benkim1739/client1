#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QImage>
#include <QJsonObject>
#include <memory>

class QLabel;
class QLineEdit;
class QPushButton;
class QComboBox;
class QTextEdit;
class RestClient;
class VideoReceiverThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    bool isConnected() const;
    void connectToServer();
    void disconnectFromServer();
    QString connectionStatus() const;

private slots:
    // UI trigger slots
    void onConnectButtonClicked();
    void onDisconnectButtonClicked();
    void onApplyFpsClicked();
    void onApplyResolutionClicked();
    void onGetStatusClicked();

    // RestClient event slots
    void onRestConnected(const QString &sessionId, const QString &rtspUrl);
    void onRestDisconnected();
    void onRestFrameRateChanged(int fps);
    void onRestResolutionChanged(int width, int height);
    void onRestStatusUpdated(const QJsonObject &status);
    void onRestError(const QString &errorCode, const QString &message);

    // VideoReceiverThread event slots
    void onVideoFrameReceived(const QImage &image);
    void onVideoStatusChanged(const QString &status);
    void onVideoError(const QString &errorCode, const QString &message);

private:
    void setupUi();
    void setupConnections();
    void appendLog(const QString &message, const QString &level = "INFO");
    void updateStatusIndicator(const QString &status);

    // UI Widgets
    QLineEdit *baseUrlEdit_;
    QLineEdit *clientIdEdit_;
    QLineEdit *clientNameEdit_;
    QPushButton *connectBtn_;
    QPushButton *disconnectBtn_;
    QComboBox *fpsCombo_;
    QPushButton *applyFpsBtn_;
    QComboBox *resCombo_;
    QPushButton *applyResBtn_;
    QPushButton *statusBtn_;

    QLabel *videoViewport_;
    QLabel *statusDot_;
    QLabel *statusLabel_;
    QTextEdit *logConsole_;

    // Core variables and worker classes
    bool connected_;
    QString statusText_;
    RestClient *restClient_;
    VideoReceiverThread *videoThread_;
};

#endif // MAINWINDOW_H
