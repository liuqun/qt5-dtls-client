#pragma once

#include <QObject>
#include <QCoreApplication>
#include <QByteArray>
#include <QString>
#include <QtNetwork>
#include <QHostAddress>
#include <QDtls>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE

//! [0]
class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = 0);
    ~Client();
    void setupWithIdAndPsk(const QHostAddress &address, quint16 port, const QString &progName, const QByteArray &idValue, const QByteArray &pskValue);
    void startHandshake();

//signals:
    //void errorMessage(const QString &message);
    //void warningMessage(const QString &message);
    //void infoMessage(const QString &message);
    //void serverResponse(const QString &clientInfo, const QByteArray &datagraam, const QByteArray &plainText);
signals:
    void clientDtlsErrorSignal();

public slots:
    void runTestCallbackSlot();
    void clientCleanupCallbackSlot();

private slots:
    void udpSocketConnected();
    void readyReadCallbackSlot();
    void handshakeTimeoutCallbackSlot();
    void pskRequiredCallbackSlot(QSslPreSharedKeyAuthenticator *auth);
    void pingTimeout();

private:
    QCoreApplication *app;
    QString name;
    QUdpSocket socket;
    QDtls crypto;
    QByteArray myIdentityByteArray;
    QByteArray myPreSharedKeyByteArray;

    QTimer pingTimer;
    unsigned ping = 0;

#ifdef Q_DISABLE_COPY
    Q_DISABLE_COPY(Client)
#endif
};
//! [0]

QT_END_NAMESPACE

// vi: set ts=4 sw=4 expandtab :
