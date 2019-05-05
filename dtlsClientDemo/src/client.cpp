#include <QCoreApplication>
#include <QDebug>
#include <QObject>

#include "client.h"

Client::Client(QObject *parent): crypto(QSslSocket::SslClientMode)
{
    // get the instance of the main application
    app = QCoreApplication::instance();
    // setup everything here
    name = "temp_name";
    // create any global objects
    // setup debug and warning mode
}

void Client::setup(const QHostAddress &address, quint16 port, const QString &connectionName)
{
    qDebug() << "Debug:setup():" << __FILE__ << ":" << __LINE__;
    auto configuration = QSslConfiguration::defaultDtlsConfiguration();
    configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
    crypto.setPeer(address, port);
    crypto.setDtlsConfiguration(configuration);

    connect(&crypto, &QDtls::handshakeTimeout, this, &Client::handshakeTimeoutCallbackSlot);
    connect(&crypto, &QDtls::pskRequired, this, &Client::pskRequiredCallbackSlot);

    pingTimer.setInterval(5000);
    connect(&pingTimer, &QTimer::timeout, this, &Client::pingTimeout);

    name = connectionName;

    socket.connectToHost(crypto.peerAddress().toString(), crypto.peerPort()); // FIXME: I am not sure how to use QUdpSocket::connectToHost(...)
    connect(&socket, &QUdpSocket::readyRead, this, &Client::readyReadCallbackSlot);
}

Client::~Client()
{
    if (crypto.isConnectionEncrypted())
        crypto.shutdown(&socket);
}

void Client::startHandshake()
{
    if (socket.state() != QAbstractSocket::ConnectedState) {
        qDebug() << tr("%1: connecting UDP socket first ...").arg(name);//emit infoMessage(tr("%1: connecting UDP socket first ...").arg(name));
        connect(&socket, &QAbstractSocket::connected, this, &Client::udpSocketConnected);
        return;
    }
    qDebug() << "Debug:" << __FILE__ << ":" << __LINE__;
    if (!crypto.doHandshake(&socket)) {
        qDebug() << tr("%1: failed to start a handshake - %2").arg(name, crypto.dtlsErrorString());//emit errorMessage(tr("%1: failed to start a handshake - %2").arg(name, crypto.dtlsErrorString()));
    } else {
        qDebug() << tr("%1: starting a handshake").arg(name);//emit infoMessage(tr("%1: starting a handshake").arg(name));
    }
}

void Client::udpSocketConnected()
{
    qDebug() << "Debug:" << __FILE__ << ":" << __LINE__;
    qDebug() << tr("%1: UDP socket is now in ConnectedState, continue with handshake ...").arg(name);//emit infoMessage(tr("%1: UDP socket is now in ConnectedState, continue with handshake ...").arg(name));
    startHandshake();
}

static const QString _parseServerResponse(const QString &clientInfo, const QByteArray &datagram, const QByteArray &plainText)
{
    return QString("---------------\n"
                "%1 received a DTLS datagram:\n%2\n"
                "As plain text:\n%3").arg(clientInfo, QString::fromUtf8(datagram.toHex(' ')), QString::fromUtf8(plainText));
}

void Client::readyReadCallbackSlot()
{
    qDebug() << "in readyReadCallbackSlot()...";
    QByteArray dgram(socket.pendingDatagramSize(), Qt::Uninitialized);
    const qint64 bytesRead = socket.readDatagram(dgram.data(), dgram.size());
    if (bytesRead <= 0) {
        qDebug() << tr("%1: spurious read notification?").arg(name);//emit warningMessage(tr("%1: spurious read notification?").arg(name));
        return;
    }

    dgram.resize(bytesRead);
    if (crypto.isConnectionEncrypted()) {
        const QByteArray plainText = crypto.decryptDatagram(&socket, dgram);
        if (plainText.size()) {
            qDebug() << _parseServerResponse(name, dgram, plainText).toStdString().c_str();//emit serverResponse(name, dgram, plainText);
            return;
        }

        if (crypto.dtlsError() == QDtlsError::RemoteClosedConnectionError) {
            qDebug() << tr("%1: shutdown alert received").arg(name);//emit errorMessage(tr("%1: shutdown alert received").arg(name));
            socket.close();
            pingTimer.stop();
            if (1) {
                qDebug() << "Server side has shutdown DTLS session... Client will quit"; // FIXME
                emit clientDtlsErrorSignal();
            }
            return;
        }

        qDebug() << tr("%1: zero-length datagram received?").arg(name);//emit warningMessage(tr("%1: zero-length datagram received?").arg(name));
    } else {
        if (!crypto.doHandshake(&socket, dgram)) {
            qDebug() << tr("%1: handshake error - %2").arg(name, crypto.dtlsErrorString());//emit errorMessage(tr("%1: handshake error - %2").arg(name, crypto.dtlsErrorString()));
            return;
        }

        if (crypto.isConnectionEncrypted()) {
            qDebug() << tr("%1: encrypted connection established!").arg(name);//emit infoMessage(tr("%1: encrypted connection established!").arg(name));
            pingTimer.start();
            pingTimeout();
        } else {
            qDebug() << tr("%1: continuing with handshake ...").arg(name);//emit infoMessage(tr("%1: continuing with handshake ...").arg(name));
        }
    }
}

void Client::handshakeTimeoutCallbackSlot()
{
    qDebug() << tr("%1: handshake timeout, trying to re-transmit").arg(name);//emit warningMessage(tr("%1: handshake timeout, trying to re-transmit").arg(name));
    if (!crypto.handleTimeout(&socket)) {
        qDebug() << tr("%1: failed to re-transmit - %2").arg(name, crypto.dtlsErrorString());//emit errorMessage(tr("%1: failed to re-transmit - %2").arg(name, crypto.dtlsErrorString()));
    }
}

void Client::pskRequiredCallbackSlot(QSslPreSharedKeyAuthenticator *auth)
{
    Q_ASSERT(auth);
    qDebug() << "in pskRequiredCallbackSlot()...";
    qDebug() << tr("%1: providing pre-shared key ...").arg(name);//emit infoMessage(tr("%1: providing pre-shared key ...").arg(name));
    auth->setIdentity(name.toLatin1());
    auth->setPreSharedKey(QByteArrayLiteral("\x1a\x2b\x3c\x4d\x5e\x6f"));
}

void Client::pingTimeout()
{
    static const QString message = QStringLiteral("I am %1, please, accept our dtls packet %2");
    const qint64 written = crypto.writeDatagramEncrypted(&socket, message.arg(name).arg(ping).toLatin1());
    if (written <= 0) {
        qDebug() << tr("%1: failed to send a ping - %2").arg(name, crypto.dtlsErrorString());//emit errorMessage(tr("%1: failed to send a ping - %2").arg(name, crypto.dtlsErrorString()));
        pingTimer.stop();
        return;
    }

    ++ping;
}

void Client::runTestCallbackSlot()
{
    qDebug() << "Debug: start handshake...";
    startHandshake();
}

void Client::clientCleanupCallbackSlot()
{
    // stop threads
    // sleep(1);   // wait for threads to stop.
    // delete any objects
    qDebug() << "receive a aboutToQuit() message...";
}

// vi: set ts=4 sw=4 expandtab :
