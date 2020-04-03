#include "QemuSocket.h"

QemuSocket::QemuSocket(QObject *parent)
    : QTcpSocket(parent)
{
    connect(this, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &QemuSocket::processError);
    connect(this, &QAbstractSocket::disconnected,
            this, &QemuSocket::disconnectedPort);
}

void QemuSocket::tryConnect()
{
#ifndef GUI
    while (state() != QAbstractSocket::ConnectedState)
    {
        connecting = true;
#endif
        connectToHost("127.0.0.1", port);
#ifndef GUI
        waitForConnected(-1);
        connecting = false;
    }
#endif
}

void QemuSocket::connectPort(int port)
{
    if (connecting)
    {
        disconnect();
    }
    this->port = port;
    connecting = true;
    tryConnect();
}

void QemuSocket::disconnect()
{
    connecting = false;
    disconnectFromHost();
}

void QemuSocket::disconnectedPort()
{
    connecting = false;
}

void QemuSocket::processError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QAbstractSocket::RemoteHostClosedError)
    {
        disconnect();
    }
#ifdef GUI
    if (connecting)
    {
        tryConnect();
    }
#endif
}
