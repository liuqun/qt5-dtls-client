#include <cmath> //std::lround()
#include <QCoreApplication>
#include <QDebug>
#include <QObject>

#include "client.h"

void Client::init(const QHostAddress &peerAddress, quint16 peerPort, const QString& name, float heartbeatIntervalInSeconds)
{
    long msec = std::lround(heartbeatIntervalInSeconds * 1000.0f);
    udpHeartbeatTimer.setInterval(static_cast<int>(msec));
    connect(&udpHeartbeatTimer, &QTimer::timeout, this, &Client::heartbeatTimerEventHandler);

    this->name = name;
    this->peerAddress = peerAddress;
    this->peerPort = peerPort;
    connect(&socket, &QUdpSocket::readyRead, this, &Client::incomingDatagramEventHandler);
}

Client::Client(QObject *parent): name("Undefined"), peerAddress("127.0.0.1"), peerPort(1024)
{
    (void)parent;
    // get the instance of the main application
    app = QCoreApplication::instance();
    // setup everything here
    // create any global objects
    // setup debug and warning mode
}

Client::~Client()
{
}

static const QString ParseServerResponse(const QString& clientInfo, const QByteArray& datagram, const QByteArray& plaintext)
{
    return QString("---------------\n"
                "%1 received a plaintext datagram:\n%2\n"
                "ASCII string:\n%3\n")
            .arg(clientInfo, QString::fromUtf8(datagram.toHex(' ')), QString::fromUtf8(plaintext));
}

void Client::incomingDatagramEventHandler()
{
    qDebug() << "in onIncomingDatagramArrived()...";
    qint64 bytesPending = socket.pendingDatagramSize();
    QByteArray dgram(static_cast<int>(bytesPending), Qt::Uninitialized);
    const qint64 bytesRead = socket.readDatagram(dgram.data(), dgram.size());
    if (bytesRead <= 0) {
        qDebug() << qPrintable(name) << ":" << qPrintable(tr("Got an unexpected socket IO error!"));
        return;
    }
    dgram.resize(static_cast<int>(bytesRead));
    const QByteArray& plaintext = dgram;
    if (plaintext.size()) {
        qDebug() << qPrintable(ParseServerResponse(name, dgram, plaintext));
        return;
    }
}

void Client::heartbeatTimerEventHandler()
{
    const QString msgFormatter =
            QStringLiteral("UDP msg from %1: counter= %2\n");
    const QByteArray& datagram = msgFormatter.arg(name).arg(counter).toLatin1();
    const qint64 written = socket.writeDatagram(datagram, peerAddress, peerPort);
    if (written <= 0) {
        qDebug() << qPrintable(name) << ":" << tr("failed to send a ping");
        udpHeartbeatTimer.stop();
        emit unexpectedSocketError();
        return;
    }
    qDebug() << qPrintable(name) << ": counter+1 =" << counter+1;
    counter += 1;
    return;
}

void Client::clientTestStartHandler()
{
    qDebug() << "Debug: send first UDP packet...";
    const QByteArray& datagram = QStringLiteral("Test start!\n").toLatin1();
    udpHeartbeatTimer.start();
    const qint64 written = socket.writeDatagram(datagram, peerAddress, peerPort);
    if (written <= 0) {
        qDebug() << qPrintable(name) << ":" << qPrintable(tr("failed to send a ping"));
        udpHeartbeatTimer.stop();
        emit unexpectedSocketError();
        return;
    }
    return;
}

void Client::clientAboutToQuitHandler()
{
    // stop threads
    // sleep(1);   // wait for threads to stop.
    // delete any objects
    qDebug() << "receive a aboutToQuit() message...";
}

// vi: set ts=4 sw=4 expandtab :
