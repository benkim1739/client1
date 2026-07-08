#include "rest_client.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

RestClient::RestClient(const QString &baseUrl, QObject *parent)
    : QObject(parent), baseUrl_(baseUrl), manager_(new QNetworkAccessManager(this))
{
}

void RestClient::setBaseUrl(const QString &url)
{
    baseUrl_ = url;
}

QString RestClient::baseUrl() const
{
    return baseUrl_;
}

QString RestClient::sessionId() const
{
    return sessionId_;
}

bool RestClient::isConnected() const
{
    return !sessionId_.isEmpty();
}

void RestClient::connectSession(const QString &clientId, const QString &clientName)
{
    clientId_ = clientId;
    QUrl url(baseUrl_ + "/sessions/connect");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["client_id"] = clientId;
    body["client_name"] = clientName;

    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);

    qDebug() << "RestClient: Sending connect request to" << url.toString();
    QNetworkReply *reply = manager_->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleConnectFinished(reply);
    });
}

void RestClient::disconnectSession()
{
    if (sessionId_.isEmpty())
    {
        emit errorOccurred("SESSION_NOT_FOUND", "No active session to disconnect");
        return;
    }

    QUrl url(baseUrl_ + "/sessions/" + sessionId_ + "/disconnect");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    qDebug() << "RestClient: Sending disconnect request to" << url.toString();
    QNetworkReply *reply = manager_->post(request, QByteArray());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleDisconnectFinished(reply);
    });
}

void RestClient::setFrameRate(int fps)
{
    if (sessionId_.isEmpty())
    {
        emit errorOccurred("SESSION_NOT_FOUND", "No active session to modify frame rate");
        return;
    }

    QUrl url(baseUrl_ + "/sessions/" + sessionId_ + "/frame-rate");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["fps"] = fps;

    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);

    qDebug() << "RestClient: Sending frame-rate request to" << url.toString() << "with FPS" << fps;
    QNetworkReply *reply = manager_->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleFrameRateFinished(reply);
    });
}

void RestClient::setResolution(int width, int height)
{
    if (sessionId_.isEmpty())
    {
        emit errorOccurred("SESSION_NOT_FOUND", "No active session to modify resolution");
        return;
    }

    QUrl url(baseUrl_ + "/sessions/" + sessionId_ + "/resolution");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject body;
    body["width"] = width;
    body["height"] = height;

    QByteArray data = QJsonDocument(body).toJson(QJsonDocument::Compact);

    qDebug() << "RestClient: Sending resolution request to" << url.toString() << "with resolution" << width << "x" << height;
    QNetworkReply *reply = manager_->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleResolutionFinished(reply);
    });
}

void RestClient::getStatus()
{
    if (sessionId_.isEmpty())
    {
        emit errorOccurred("SESSION_NOT_FOUND", "No active session to get status");
        return;
    }

    QUrl url(baseUrl_ + "/sessions/" + sessionId_ + "/status");
    QNetworkRequest request(url);

    qDebug() << "RestClient: Sending status request to" << url.toString();
    QNetworkReply *reply = manager_->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleStatusFinished(reply);
    });
}

void RestClient::handleConnectFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError)
    {
        handleErrorResponse(reply, data);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    QString sessId;
    QString rtspUrl;

    if (obj.contains("session_id"))
    {
        sessId = obj.value("session_id").toString();
        rtspUrl = obj.value("rtsp_url").toString();
    }
    else if (obj.contains("result"))
    {
        QJsonObject result = obj.value("result").toObject();
        if (result.contains("data"))
        {
            QJsonObject dataObj = result.value("data").toObject();
            sessId = dataObj.value("session_id").toString();
            rtspUrl = dataObj.value("rtsp_url").toString();
        }
    }

    if (sessId.isEmpty())
    {
        emit errorOccurred("PARSE_ERROR", "Failed to parse session_id from connect response");
        return;
    }

    sessionId_ = sessId;
    qDebug() << "RestClient: Successfully connected. Session ID:" << sessionId_ << "RTSP URL:" << rtspUrl;
    emit connected(sessionId_, rtspUrl);
}

void RestClient::handleDisconnectFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError)
    {
        handleErrorResponse(reply, data);
        return;
    }

    sessionId_.clear();
    qDebug() << "RestClient: Successfully disconnected.";
    emit disconnected();
}

void RestClient::handleFrameRateFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError)
    {
        handleErrorResponse(reply, data);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    int fps = 0;
    if (obj.contains("frame_rate"))
    {
        fps = obj.value("frame_rate").toInt();
    }
    else if (obj.contains("result"))
    {
        QJsonObject result = obj.value("result").toObject();
        if (result.contains("data"))
        {
            QJsonObject dataObj = result.value("data").toObject();
            fps = dataObj.value("frame_rate").toInt();
        }
    }

    qDebug() << "RestClient: Frame rate changed successfully to" << fps;
    emit frameRateChanged(fps);
}

void RestClient::handleResolutionFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError)
    {
        handleErrorResponse(reply, data);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    int width = 0;
    int height = 0;
    if (obj.contains("resolution"))
    {
        QJsonObject resObj = obj.value("resolution").toObject();
        width = resObj.value("width").toInt();
        height = resObj.value("height").toInt();
    }
    else if (obj.contains("result"))
    {
        QJsonObject result = obj.value("result").toObject();
        if (result.contains("data"))
        {
            QJsonObject dataObj = result.value("data").toObject();
            if (dataObj.contains("resolution"))
            {
                QJsonObject resObj = dataObj.value("resolution").toObject();
                width = resObj.value("width").toInt();
                height = resObj.value("height").toInt();
            }
        }
    }

    qDebug() << "RestClient: Resolution changed successfully to" << width << "x" << height;
    emit resolutionChanged(width, height);
}

void RestClient::handleStatusFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    QByteArray data = reply->readAll();

    if (reply->error() != QNetworkReply::NoError)
    {
        handleErrorResponse(reply, data);
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    QJsonObject statusObj = obj;

    if (obj.contains("result"))
    {
        QJsonObject result = obj.value("result").toObject();
        if (result.contains("data"))
        {
            statusObj = result.value("data").toObject();
        }
    }

    qDebug() << "RestClient: Received status update.";
    emit statusUpdated(statusObj);
}

void RestClient::handleErrorResponse(QNetworkReply *reply, const QByteArray &data)
{
    QString code = "HTTP_ERROR_" + QString::number(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    QString message = reply->errorString();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isNull() && doc.isObject())
    {
        QJsonObject obj = doc.object();
        if (obj.contains("error") && obj.value("error").isObject())
        {
            QJsonObject errorObj = obj.value("error").toObject();
            code = errorObj.value("code").toString(code);
            message = errorObj.value("message").toString(message);
        }
    }

    qDebug() << "RestClient: Error occurred -" << code << ":" << message;
    emit errorOccurred(code, message);
}
