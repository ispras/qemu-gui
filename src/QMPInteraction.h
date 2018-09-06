#ifndef QMPINTERACTION_H
#define QMPINTERACTION_H

#include <QWidget>
#include <QtWidgets>
#include "QemuSocket.h"


class QMPInteraction : public QObject
{
    Q_OBJECT

public:
    QMPInteraction(QObject *parent, int port);
    ~QMPInteraction();

private:
    QemuSocket socket;

private:
    QByteArray init();
    QByteArray cmd_stop();
    QByteArray cmd_continue();
    QByteArray cmd_shutdown();
    void what_said_qmp(QByteArray message);

public slots:
    void read_terminal();
    void connectedSocket();
    void command_stop_qemu();
    void command_resume_qemu();
    void command_shutdown_qemu();

signals :
    void qemu_resumed();
    void qemu_stopped();

};


#endif // QMPINTERACTION_H

