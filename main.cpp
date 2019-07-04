
#include <QObject>
#include <QTimer>
#include "mydtlsclienthandler.h"
#include "myapplication.h"

int main(int argc, char *argv[])
{
    MyApp app(argc, argv);

    MyDtlsClientHandler handler;

    QString progName = "MyProg001";
    quint16 port = 22334;
    QString ipAddrString = "192.168.1.16";
    QHostAddress serverIpAddr(ipAddrString);
    MyDtlsClientHandler handler;
    handler.setup(serverIpAddr, port, progName);

    //QObject::connect(&handler, SIGNAL(infoMessage()), 
    //         &app, SLOT(printConsole()));

    // This code will start the messaging engine in QT and in
    // 10ms it will start the execution in the MainClass.run routine;
    QTimer::singleShot(10, &handler, SLOT(run())); 
    return app.exec();
}
 



#if 0
#include <QtNetwork>
#include <QHostAddress>
//#include <QDtls>
//#include <QUdpSocket>
//#include <QSslConfiguration>
//#include <QSslSocket>
//#include <QObject>

#include "mydtlsclienthandler.h"


int main(int argc, char *argv[])
{
    //auto conf = QSslConfiguration::defaultDtlsConfiguration();
    //conf.setPeerVerifyMode(
    //        QSslSocket::VerifyNone /* FIXME: Replace with other verify mode (eg: CA mode) */ );
    QString ipAddrString = "192.168.1.16";
    QHostAddress serverIpAddr(ipAddrString);
    quint16 port = 22334;
    //QDtls dtlsSession(QSslSocket::SslClientMode);
    //dtlsSession.setPeer(serverIpAddr, port);
    //dtlsSession.setDtlsConfiguration(conf);
    //QUdpSocket clientSocket;
    QString progName = "MyProg001";
    MyDtlsClientHandler handler;
    handler.setup(serverIpAddr, port, progName);
    QCoreApplication a(argc, argv);

    return a.exec();
}
#endif
// vi: set ts=4 sw=4 expandtab :
