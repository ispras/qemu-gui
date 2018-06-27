#ifndef QEMU_SOCKET_H
#define QEMU_SOCKET_H

#include <QTcpSocket>

class QemuSocket : public QTcpSocket
{
public:
    QemuSocket(QObject *parent = NULL);

    void connectPort(int port);
    void disconnect();
private:
    void tryConnect();
    void processError(QAbstractSocket::SocketError socketError);
    void disconnectedPort();

private:
    int port;
    bool connecting;
};

#endif
