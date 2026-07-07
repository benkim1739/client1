#include "mainwindow.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), connected_(false), statusText_("Disconnected"), statusLabel_(nullptr)
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);

    statusLabel_ = new QLabel(statusText_, this);
    statusLabel_->setObjectName("statusLabel");
    layout->addWidget(statusLabel_);

    auto *buttonLayout = new QHBoxLayout();
    auto *connectButton = new QPushButton("Connect", this);
    auto *disconnectButton = new QPushButton("Disconnect", this);
    buttonLayout->addWidget(connectButton);
    buttonLayout->addWidget(disconnectButton);
    layout->addLayout(buttonLayout);

    connect(connectButton, &QPushButton::clicked, this, &MainWindow::connectToServer);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectFromServer);

    setCentralWidget(central);
    resize(480, 240);
}

bool MainWindow::isConnected() const
{
    return connected_;
}

void MainWindow::connectToServer()
{
    connected_ = true;
    statusText_ = "Connected";
    if (statusLabel_)
    {
        statusLabel_->setText(statusText_);
    }
}

void MainWindow::disconnectFromServer()
{
    connected_ = false;
    statusText_ = "Disconnected";
    if (statusLabel_)
    {
        statusLabel_->setText(statusText_);
    }
}

QString MainWindow::connectionStatus() const
{
    return statusText_;
}
