#include "QMPInteraction.h"


QMPInteraction::QMPInteraction(QObject *parent, int port)
    : QObject(parent), isQmpConnect(false)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read_terminal()));
    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSocket()));

    socket.connectPort(port);
}

QMPInteraction::~QMPInteraction()
{
}

QByteArray QMPInteraction::init()
{
    return "{ \"execute\": \"qmp_capabilities\" }";
}

QByteArray QMPInteraction::cmd_stop()
{
    return "{ \"execute\": \"stop\" }";
}

QByteArray QMPInteraction::cmd_continue()
{
    return "{ \"execute\": \"cont\" }";
}

QByteArray QMPInteraction::cmd_shutdown()
{
    return "{ \"execute\": \"quit\" }";
}

void QMPInteraction::what_said_qmp(QByteArray message)
{
    if (!isQmpConnect)
    {
        isQmpConnect = true;
        emit connectionEstablished();
    }
    qDebug() << "QMP: " << message;
    QJsonDocument qmp_message = QJsonDocument::fromJson(message);
    QJsonObject obj = qmp_message.object();
    QJsonValue json_command = obj["event"];
    qDebug() << json_command;
    QString command = json_command.toString();
    if (command.compare("resume", Qt::CaseInsensitive) == 0)
    {
        emit qemu_resumed();
    }
    if (command.compare("stop", Qt::CaseInsensitive) == 0)
    {
        emit qemu_stopped();
    }
}

void QMPInteraction::read_terminal()
{
    QByteArray message = socket.readAll();
    what_said_qmp(message);
}

void QMPInteraction::connectedSocket()
{
    socket.write(init());
}

void QMPInteraction::command_stop_qemu()
{
    socket.write(cmd_stop());
}

void QMPInteraction::command_resume_qemu()
{
    socket.write(cmd_continue());
}

void QMPInteraction::commandShutdownQemu()
{
    socket.write(cmd_shutdown());
}


/******************************************************************************
* QMP Interaction Settings                                                    *
******************************************************************************/


QMPInteractionSettings::QMPInteractionSettings(QObject *parent, int port)
    : QMPInteraction(parent)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read_terminal()));
    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSocket()));

    isQmpReady = false;

    socket.connectPort(port);
    messageBegin = "";
}

QMPInteractionSettings::~QMPInteractionSettings()
{
}

QByteArray QMPInteractionSettings::cmdMachineInfo()
{
    return "{ \"execute\": \"query-machines\" }";
}

QByteArray QMPInteractionSettings::cmdCpuInfo()
{
    return "{ \"execute\": \"query-cpu-definitions\" }";
}

bool QMPInteractionSettings::what_said_qmp(QByteArray message)
{
    QJsonDocument qmp_message = QJsonDocument::fromJson(message);
    if (qmp_message.isNull())
    {
        return false;
    }
    QJsonObject root = qmp_message.object();
    QJsonValue jv = root.value("return");
    if (jv.isArray())
    {
        infoList.clear();
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            QString str = v.toObject()["name"].toString();
            infoList.append(str);
        }
        emit readyInfo(infoList);
        messageBegin = "";
    }
    return true;
}

void QMPInteractionSettings::read_terminal()
{
    QByteArray message = messageBegin + socket.readAll();
    if (!what_said_qmp(message))
    {
        messageBegin = message;
    }
    else
    {
        messageBegin = "";
    }
    if (!isQmpReady)
    {
        emit qmpConnected();
        isQmpReady = true;
    }
}

void QMPInteractionSettings::connectedSocket()
{
    socket.write(init());
}

void QMPInteractionSettings::commandShutdownQemu()
{
    socket.write(cmd_shutdown());
}

void QMPInteractionSettings::commandCpuInfo()
{
    socket.write(cmdCpuInfo());
}

void QMPInteractionSettings::commandMachineInfo()
{
    socket.write(cmdMachineInfo());
}

