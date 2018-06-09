#include "QMPInteraction.h"


QMPInteraction::QMPInteraction(QObject *parent)
    : QObject(parent)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read_terminal()));

    socket.connectToHost("127.0.0.1", 23);
    socket.write(init());
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

void QMPInteraction::what_said_qmp(QByteArray message)
{
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

void QMPInteraction::command_stop_qemu()
{
    socket.write(cmd_stop());
}

void QMPInteraction::command_resume_qemu()
{
    socket.write(cmd_continue());
}

