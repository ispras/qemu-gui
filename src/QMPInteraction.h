#ifndef QMPINTERACTION_H
#define QMPINTERACTION_H

#include <QWidget>
#include <QtWidgets>
#include "QemuSocket.h"


class QMPInteraction : public QObject
{
    Q_OBJECT

public:
    QMPInteraction(QObject *parent) {};
    QMPInteraction(QObject *parent, int port);
    ~QMPInteraction();

private:
    QemuSocket socket;

protected:
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
    void commandShutdownQemu();

signals :
    void qemu_resumed();
    void qemu_stopped();
};


class QMPInteractionSettings : public QMPInteraction
{
    Q_OBJECT

public:
    QMPInteractionSettings(QObject *parent, int port);
    ~QMPInteractionSettings();

private:
    QemuSocket socket;
    bool isQmpReady;
    QStringList infoList;
    QByteArray messageBegin;

private:
    QByteArray cmdMachineInfo();
    QByteArray cmdCpuInfo();

    bool what_said_qmp(QByteArray message);

public slots:
    void read_terminal();
    void connectedSocket();

    void commandCpuInfo();
    void commandMachineInfo();
    void commandShutdownQemu();

signals:
    void readyInfo(const QStringList &);

    void qmpConnected();

};


#endif // QMPINTERACTION_H

