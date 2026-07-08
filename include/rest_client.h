#ifndef REST_CLIENT_H
#define REST_CLIENT_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class RestClient : public QObject
{
    Q_OBJECT

public:
    explicit RestClient(const QString &baseUrl = "", QObject *parent = nullptr);
    virtual ~RestClient() = default;

    void setBaseUrl(const QString &url);
    QString baseUrl() const;
    QString sessionId() const;
    bool isConnected() const;

    void connectSession(const QString &clientId, const QString &clientName);
    void disconnectSession();
    void setFrameRate(int fps);
    void setResolution(int width, int height);
    void getStatus();

signals:
    void connected(const QString &sessionId, const QString &rtspUrl);
    void disconnected();
    void frameRateChanged(int fps);
    void resolutionChanged(int width, int height);
    void statusUpdated(const QJsonObject &status);
    void errorOccurred(const QString &errorCode, const QString &message);

private slots:
    void handleConnectFinished(QNetworkReply *reply);
    void handleDisconnectFinished(QNetworkReply *reply);
    void handleFrameRateFinished(QNetworkReply *reply);
    void handleResolutionFinished(QNetworkReply *reply);
    void handleStatusFinished(QNetworkReply *reply);

private:
    QString baseUrl_;
    QString sessionId_;
    QString clientId_;
    QNetworkAccessManager *manager_;

    void handleErrorResponse(QNetworkReply *reply, const QByteArray &data);
};

#endif // REST_CLIENT_H
