#pragma once

//#include <QByteArray>
#include <QCoreApplication>
#include <QObject>
#include <QString>

QT_BEGIN_NAMESPACE

//! [0]
class MyApp : public QCoreApplication
{
    Q_OBJECT

public:
    MyApp(int argc, char *argv);
    ~MyApp();
    void setup(const QHostAddress &address, quint16 port, const QString &connectionName);
    void startHandshake();

//signals:
    //void errorMessage(const QString &message);
    //void warningMessage(const QString &message);
    //void infoMessage(const QString &message);
    //void serverResponse(const QString &clientInfo, const QByteArray &datagraam, const QByteArray &plainText);

private slots:
    void udpSocketConnected();
    void readyReadCallbackSlot();
    void handshakeTimeoutCallbackSlot();
    void pskRequiredCallbackSlot(QSslPreSharedKeyAuthenticator *auth);
    void pingTimeout();

private:
    QString name;
    QUdpSocket socket;
    QDtls crypto;

    QTimer pingTimer;
    unsigned ping = 0;

#ifdef Q_DISABLE_COPY
    Q_DISABLE_COPY(MyApp)
#endif
};
//! [0]

QT_END_NAMESPACE

// vi: set ts=4 sw=4 expandtab :
