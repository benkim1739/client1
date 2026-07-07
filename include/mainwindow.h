#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    bool isConnected() const;
    void connectToServer();
    void disconnectFromServer();
    QString connectionStatus() const;

private:
    bool connected_;
    QString statusText_;
    QLabel *statusLabel_;
};

#endif
