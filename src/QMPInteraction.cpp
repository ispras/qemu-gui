#include "QMPInteraction.h"


QMPInteraction::QMPInteraction(QObject *parent)
    : QObject(parent)
{
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

int QMPInteraction::what_said_qmp(QByteArray message)
{
    /* enum VMState {None, Running, Stopped}; */
    QString msg = message;
    if (msg.contains("event", Qt::CaseInsensitive))
    {
        if (msg.contains("stop", Qt::CaseInsensitive))
        {
            return 2;
        }
        if (msg.contains("resume", Qt::CaseInsensitive))
        {
            return 1;
        }
    }
    return 0;
}

