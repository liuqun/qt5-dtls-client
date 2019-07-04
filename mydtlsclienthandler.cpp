
#include "mydtlsclienthandler.h"

MyDtlsClientHandler::MyDtlsClientHandler(): crypto(QSslSocket::SslClientMode)
{
    name = "temp_prog_name";
}

void MyDtlsClientHandler::setup(const QHostAddress &address, quint16 port, const QString &connectionName)
{
    auto configuration = QSslConfiguration::defaultDtlsConfiguration();
    configuration.setPeerVerifyMode(QSslSocket::VerifyNone);
    crypto.setPeer(address, port);
    crypto.setDtlsConfiguration(configuration);

    connect(&crypto, &QDtls::handshakeTimeout, this, &MyDtlsClientHandler::handshakeTimeoutCallbackSlot);
    connect(&crypto, &QDtls::pskRequired, this, &MyDtlsClientHandler::pskRequiredCallbackSlot);
    socket.connectToHost(address.toString(), port);
    connect(&socket, &QUdpSocket::readyRead, this, &MyDtlsClientHandler::readyReadCallbackSlot);
    pingTimer.setInterval(5000);
    connect(&pingTimer, &QTimer::timeout, this, &MyDtlsClientHandler::pingTimeout);

    name = connectionName;
}

MyDtlsClientHandler::~MyDtlsClientHandler()
{
    if (crypto.isConnectionEncrypted())
        crypto.shutdown(&socket);
}

void MyDtlsClientHandler::startHandshake()
{
    if (socket.state() != QAbstractSocket::ConnectedState) {
        emit infoMessage(tr("%1: connecting UDP socket first ...").arg(name));
        connect(&socket, &QAbstractSocket::connected, this, &MyDtlsClientHandler::udpSocketConnected);
        return;
    }

    if (!crypto.doHandshake(&socket)) {
        //emit errorMessage(tr("%1: failed to start a handshake - %2").arg(name, crypto.dtlsErrorString()));
        emit infoMessage(tr("%1: failed to start a handshake - %2").arg(name, crypto.dtlsErrorString()));
    } else {
        emit infoMessage(tr("%1: starting a handshake").arg(name));
    }
}

void MyDtlsClientHandler::udpSocketConnected()
{
    emit infoMessage(tr("%1: UDP socket is now in ConnectedState, continue with handshake ...").arg(name));
    startHandshake();
}

void MyDtlsClientHandler::readyReadCallbackSlot()
{
    QByteArray dgram(socket.pendingDatagramSize(), Qt::Uninitialized);
    const qint64 bytesRead = socket.readDatagram(dgram.data(), dgram.size());
    if (bytesRead <= 0) {
        //emit warningMessage(tr("%1: spurious read notification?").arg(name));
        return;
    }

    dgram.resize(bytesRead);
    if (crypto.isConnectionEncrypted()) {
        const QByteArray plainText = crypto.decryptDatagram(&socket, dgram);
        if (plainText.size()) {
            emit serverResponse(name, dgram, plainText);
            return;
        }

        if (crypto.dtlsError() == QDtlsError::RemoteClosedConnectionError) {
            //emit errorMessage(tr("%1: shutdown alert received").arg(name));
            emit infoMessage(tr("%1: shutdown alert received").arg(name));
            socket.close();
            pingTimer.stop();
            return;
        }

        //emit warningMessage(tr("%1: zero-length datagram received?").arg(name));
    } else {
        if (!crypto.doHandshake(&socket, dgram)) {
            //emit errorMessage(tr("%1: handshake error - %2").arg(name, crypto.dtlsErrorString()));
            emit infoMessage(tr("%1: handshake error - %2").arg(name, crypto.dtlsErrorString()));
            return;
        }

        if (crypto.isConnectionEncrypted()) {
            emit infoMessage(tr("%1: encrypted connection established!").arg(name));
            pingTimer.start();
            pingTimeout();
        } else {
            emit infoMessage(tr("%1: continuing with handshake ...").arg(name));
        }
    }
}

void MyDtlsClientHandler::handshakeTimeoutCallbackSlot()
{
    //emit warningMessage(tr("%1: handshake timeout, trying to re-transmit").arg(name));
    emit infoMessage(tr("%1: handshake timeout, trying to re-transmit").arg(name));
    if (!crypto.handleTimeout(&socket)) {
        //emit errorMessage(tr("%1: failed to re-transmit - %2").arg(name, crypto.dtlsErrorString()));
        emit infoMessage(tr("%1: failed to re-transmit - %2").arg(name, crypto.dtlsErrorString()));
    }
}

void MyDtlsClientHandler::pskRequiredCallbackSlot(QSslPreSharedKeyAuthenticator *auth)
{
    Q_ASSERT(auth);

    emit infoMessage(tr("%1: providing pre-shared key ...").arg(name));
    auth->setIdentity(name.toLatin1());
    auth->setPreSharedKey(QByteArrayLiteral("\x1a\x2b\x3c\x4d\x5e\x6f"));
}

void MyDtlsClientHandler::pingTimeout()
{
    static const QString message = QStringLiteral("I am %1, please, accept our dtls packet %2");
    const qint64 written = crypto.writeDatagramEncrypted(&socket, message.arg(name).arg(ping).toLatin1());
    if (written <= 0) {
        //emit errorMessage(tr("%1: failed to send a ping - %2").arg(name, crypto.dtlsErrorString()));
        pingTimer.stop();
        return;
    }

    ++ping;
}
