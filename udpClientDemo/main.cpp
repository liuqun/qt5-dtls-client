#include <QCoreApplication>
#include <QTimer>
#include <QHostAddress>

#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString clientName("DemoUdpClient");
    quint16 remotePort = 1025;
    QHostAddress remoteIpv4Addr((argc>=2)? argv[1]:"127.0.0.1");

    Client client(nullptr);
    float heartbeatInterval = 5;
    client.init(remoteIpv4Addr, remotePort, clientName, heartbeatInterval);

    QObject::connect(&client, SIGNAL(unexpectedSocketError()), &app, SLOT(quit()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), &client, SLOT(clientAboutToQuitHandler()));

    // This code will start the messaging engine in QT and in 100ms it will start the execution in the Client::clientTestStartHandler() routine;
    QTimer::singleShot(100, &client, SLOT(clientTestStartHandler()));
    return app.exec();
}
// vi: set ts=4 sw=4 expandtab :
