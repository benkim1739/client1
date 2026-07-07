#include "mainwindow.h"

#include <QApplication>
#include <gtest/gtest.h>

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

TEST(MainWindowTest, ConnectAndDisconnectUpdatesState)
{
    ensureApplication();
    MainWindow window;

    window.connectToServer();
    EXPECT_TRUE(window.isConnected());
    EXPECT_EQ(window.connectionStatus(), QString("Connected"));

    window.disconnectFromServer();
    EXPECT_FALSE(window.isConnected());
    EXPECT_EQ(window.connectionStatus(), QString("Disconnected"));
}
