#include <QCoreApplication>
#include <QTimer>
#include "client.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString progName = "MyClient001";
    quint16 remotePort = 22334;
    const char *str = (argc>=2)? argv[1]:"192.168.1.106";
    QHostAddress remoteIpv4Addr(str);

    Client client;
    client.setup(remoteIpv4Addr, remotePort, progName);

    QObject::connect(&client, SIGNAL(clientDtlsErrorSignal()), &app, SLOT(quit()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), &client, SLOT(clientCleanupCallbackSlot()));

    // This code will start the messaging engine in QT and in 10ms it will start the execution in the Client.run routine;
    QTimer::singleShot(10, &client, SLOT(runTestCallbackSlot()));
    return app.exec();
}
