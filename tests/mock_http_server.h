#ifndef MOCK_HTTP_SERVER_H
#define MOCK_HTTP_SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QDebug>

class MockHttpServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MockHttpServer(QObject *parent = nullptr)
        : QTcpServer(parent), lastStatus(200)
    {
        connect(this, &QTcpServer::newConnection, this, &MockHttpServer::handleNewConnection);
    }

    QString lastRequestPath;
    QByteArray lastRequestBody;
    QJsonObject lastRequestJson;
    int lastStatus;

    void reset()
    {
        lastRequestPath.clear();
        lastRequestBody.clear();
        lastRequestJson = QJsonObject();
        lastStatus = 200;
    }

private slots:
    void handleNewConnection()
    {
        QTcpSocket *socket = nextPendingConnection();
        if (!socket) return;

        connect(socket, &QTcpSocket::readyRead, this, [this, socket]() {
            QByteArray data = socket->readAll();
            int headerEnd = data.indexOf("\r\n\r\n");
            if (headerEnd != -1)
            {
                QByteArray headerData = data.left(headerEnd);
                lastRequestBody = data.mid(headerEnd + 4);

                // Parse path
                int firstSpace = headerData.indexOf(' ');
                int secondSpace = headerData.indexOf(' ', firstSpace + 1);
                if (firstSpace != -1 && secondSpace != -1)
                {
                    lastRequestPath = QString::fromUtf8(headerData.mid(firstSpace + 1, secondSpace - firstSpace - 1));
                }

                // Parse JSON body if present
                if (!lastRequestBody.isEmpty())
                {
                    QJsonDocument doc = QJsonDocument::fromJson(lastRequestBody);
                    if (doc.isObject())
                    {
                        lastRequestJson = doc.object();
                    }
                }
            }

            // Route and generate response
            QByteArray responseBody;
            int status = 200;
            QString statusMsg = "OK";

            if (lastRequestPath == "/api/v1/sessions/connect")
            {
                QJsonObject resp;
                resp["session_id"] = "sess-abc123";
                resp["client_id"] = lastRequestJson.value("client_id").toString("client-001");
                resp["rtsp_url"] = "rtsp://127.0.0.1:8554/camera";
                resp["connected_at"] = "2026-07-08T13:31:00Z";
                responseBody = QJsonDocument(resp).toJson(QJsonDocument::Compact);
            }
            else if (lastRequestPath == "/api/v1/sessions/sess-abc123/disconnect")
            {
                QJsonObject resp;
                resp["session_id"] = "sess-abc123";
                resp["disconnected_at"] = "2026-07-08T13:32:00Z";
                responseBody = QJsonDocument(resp).toJson(QJsonDocument::Compact);
            }
            else if (lastRequestPath == "/api/v1/sessions/sess-abc123/frame-rate")
            {
                QJsonObject resp;
                resp["session_id"] = "sess-abc123";
                resp["frame_rate"] = lastRequestJson.value("fps").toInt(30);
                resp["applied_at"] = "2026-07-08T13:32:10Z";
                responseBody = QJsonDocument(resp).toJson(QJsonDocument::Compact);
            }
            else if (lastRequestPath == "/api/v1/sessions/sess-abc123/resolution")
            {
                QJsonObject resp;
                resp["session_id"] = "sess-abc123";
                QJsonObject res;
                res["width"] = lastRequestJson.value("width").toInt(1920);
                res["height"] = lastRequestJson.value("height").toInt(1080);
                resp["resolution"] = res;
                resp["applied_at"] = "2026-07-08T13:32:20Z";
                responseBody = QJsonDocument(resp).toJson(QJsonDocument::Compact);
            }
            else if (lastRequestPath == "/api/v1/sessions/sess-abc123/status")
            {
                QJsonObject resp;
                resp["session_id"] = "sess-abc123";
                resp["client_id"] = "client-001";
                resp["connection_status"] = "CONNECTED";
                resp["stream_status"] = "STREAMING";
                QJsonObject res;
                res["width"] = 1920;
                res["height"] = 1080;
                resp["current_resolution"] = res;
                resp["current_fps"] = 30;
                resp["current_codec"] = "H.264";
                responseBody = QJsonDocument(resp).toJson(QJsonDocument::Compact);
            }
            else
            {
                status = 404;
                statusMsg = "Not Found";
                QJsonObject resp;
                QJsonObject err;
                err["code"] = "ENDPOINT_NOT_FOUND";
                err["message"] = "The requested endpoint was not found";
                resp["error"] = err;
                responseBody = QJsonDocument(resp).toJson(QJsonDocument::Compact);
            }

            lastStatus = status;

            QByteArray response;
            response.append(QString("HTTP/1.1 %1 %2\r\n").arg(QString::number(status), statusMsg).toUtf8());
            response.append("Content-Type: application/json\r\n");
            response.append(QString("Content-Length: %1\r\n").arg(responseBody.length()).toUtf8());
            response.append("Connection: close\r\n\r\n");
            response.append(responseBody);

            socket->write(response);
            socket->disconnectFromHost();
        });

        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
};

#endif // MOCK_HTTP_SERVER_H
