#pragma once

#include <QObject>
#include <QCoreApplication>
#include <QByteArray>
#include <QString>
#include <QtNetwork>
#include <QHostAddress>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    ~Client();
    void init(const QHostAddress &peerAddress, quint16 peerPort, const QString& name, float heartbeatIntervalInSeconds=3.0);
signals:
    void unexpectedSocketError();

public slots:
    void clientTestStartHandler();
    void clientAboutToQuitHandler();

private slots:
    void incomingDatagramEventHandler();
    void heartbeatTimerEventHandler();

private:
    QCoreApplication *app;
    QString name;
    QUdpSocket socket;
    QHostAddress peerAddress;
    quint16 peerPort;
    QTimer udpHeartbeatTimer;
    unsigned counter = 0;

#ifdef Q_DISABLE_COPY
    Q_DISABLE_COPY(Client)
#endif
};


QT_END_NAMESPACE

// vi: set ts=4 sw=4 expandtab :
