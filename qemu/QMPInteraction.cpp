#include "QMPInteraction.h"

QMPInteraction::QMPInteraction(QObject *parent)
    : QObject(parent)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(readTerminal()));
    prepareCommands();
}

QMPInteraction::QMPInteraction(QObject *parent, int port)
    : QMPInteraction(parent)
{
    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSocket()));

    socket.connectPort(port);
}

QMPInteraction::~QMPInteraction()
{
}


void QMPInteraction::prepareCommands()
{
    cmdMap.insert(QMPCommands::Continue, { "cont", "", &QMPInteraction::continue_cb });
    cmdMap.insert(QMPCommands::Stop, { "stop", "", &QMPInteraction::stop_cb });
    cmdMap.insert(QMPCommands::Quit, { "quit", "", NULL });
    cmdMap.insert(QMPCommands::QmpCapabilities, { "qmp_capabilities", "", &QMPInteraction::dummy_cb });
    cmdMap.insert(QMPCommands::QueryStatus,{ "query-status", "", &QMPInteraction::queryStatus_cb });
    cmdMap.insert(QMPCommands::QueryMachines, { "query-machines", "",
        &QMPInteraction::machine_cb });
    cmdMap.insert(QMPCommands::QueryCpuDefinitions, { "query-cpu-definitions", "",
        &QMPInteraction::cpu_cb });
    cmdMap.insert(QMPCommands::QomListTypes, { "qom-list-types",
        ",\"arguments\":{\"abstract\":true, \"implements\":\"device\"}",
        &QMPInteraction::listDevices_cb });
    cmdMap.insert(QMPCommands::DeviceListProperties, { "device-list-properties", "?",
        &QMPInteraction::listProperties_cb });
}

void QMPInteraction::dummy_cb(QJsonObject object)
{
    if (!isEvent(object))
    {
        cbQueue.pop_front();
    }
}

bool QMPInteraction::isEvent(QJsonObject object)
{
    QJsonValue json_command = object["event"];
    if (!json_command.isNull())
    {
        cbQueue.pop_front();
        return true;
    }
    return false;
}

void QMPInteraction::queryStatus_cb(QJsonObject object)
{
    QJsonValue json_command = object["return"];
    if (!json_command.toObject()["running"].isNull())
    {
        bool runStatus = json_command.toObject()["running"].toBool();
        emit qemuRunningStatus(runStatus);
        cbQueue.pop_front();
    }
}

void QMPInteraction::stop_cb(QJsonObject object)
{
    if (isEvent(object))
    {
        emit qemuStopped();
    }
}

void QMPInteraction::continue_cb(QJsonObject object)
{
    if (isEvent(object))
    {
        emit qemuResumed();
    }
}

bool QMPInteraction::whatSaidQmp(QByteArray message)
{
    QJsonParseError err;
    QJsonDocument qmp_message = QJsonDocument::fromJson(message, &err);
    //qDebug() << qmp_message;
    if (qmp_message.isNull())
    {
        return false;
    }
    QJsonObject obj = qmp_message.object();

    if (!cbQueue.isEmpty())
    {
        (this->*(cbQueue.first()))(obj);
        if (cbQueue.empty())
        {
            emit ready();
        }
    }
    return true;
}

void QMPInteraction::readTerminal()
{
    QByteArray message = messageBegin + socket.readAll();
    messageBegin.clear();
    // if (!message.isEmpty())
    // {
    //     QByteArray tmp = message;
    //     tmp.truncate(80);
    //     qDebug() << "\n\nSocket data:\n\n" << tmp;
    // }
    QList<QByteArray> messageBuffer = message.split('\n');
    bool last = false;
    foreach(QByteArray message, messageBuffer)
    {
        Q_ASSERT(!last);
        if (!whatSaidQmp(message))
        {
            messageBegin = message;
            /* is this the last one? */
            last = true;
        }
    }
}

void QMPInteraction::connectedSocket()
{
    commandQmp(QMPCommands::QmpCapabilities);
    commandQmp(QMPCommands::QueryStatus);
    // Send everything
    while (!commandsQueue.isEmpty())
    {
        commandQmp(commandsQueue.first());
        commandsQueue.removeFirst();
    }
}

void QMPInteraction::commandQmp(QMPCommands cmd, const QString &specParams)
{
    QmpCommand command = cmdMap.value(cmd);
    if (command.callback)
    {
        cbQueue.append(command.callback);
    }
    QString params = specParams.isEmpty() ? command.params : specParams;
    QByteArray commandAll = "{ \"execute\":\"" + command.command.toLocal8Bit() + "\""
        + params.toLocal8Bit() + "}";
    qDebug() << "QMP send " << commandAll;
    socket.write(commandAll);
#ifndef GUI
    socket.flush();
    while (!cbQueue.empty())
    {
        socket.waitForReadyRead(-1);
        readTerminal();
    }
#endif
}


/******************************************************************************
* QMP Interaction Settings                                                    *
******************************************************************************/


QMPInteractionSettings::QMPInteractionSettings(QObject *parent, int port,
    PlatformInfo *pi)
    : QMPInteraction(parent), platformInfo(pi)
{
    commandsQueue.append({ QMPCommands::QueryMachines,
                        QMPCommands::QueryCpuDefinitions,
                        QMPCommands::QomListTypes,
    });

    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSocket()));
    socket.connectPort(port);
}

void QMPInteractionSettings::machine_cb(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            QString name = v.toObject()["name"].toString();
            bool isDefault = v.toObject()["is-default"].toBool();
            platformInfo->addMachine(name, isDefault);
        }
        cbQueue.pop_front();
    }
}

void QMPInteractionSettings::cpu_cb(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            QString name = v.toObject()["name"].toString();
            platformInfo->addCpu(name);
        }
        cbQueue.pop_front();
    }
}

void QMPInteractionSettings::listDevices_cb(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        // Remove it, because we continue sending commands
        cbQueue.pop_front();
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            bool isAbstract = v.toObject()["abstract"].toBool();
            QString deviceName = v.toObject()["name"].toString();
            if (!isAbstract)
            {
                devices.append(deviceName);
                QMPInteraction::commandQmp(QMPCommands::DeviceListProperties,
                    getParamDevListProperties(deviceName));
            }
        }
    }
}

void QMPInteractionSettings::listProperties_cb(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            QString name = v.toObject()["name"].toString();
            if (name == "netdev")
            {
                platformInfo->addNetdev(devices.first());
            }
        }
        devices.pop_front();
        cbQueue.pop_front();
    }
}

QString QMPInteractionSettings::getParamDevListProperties(const QString &name) const
{
    return ",\"arguments\":{\"typename\":\"" + name + "\"}";
}

void QMPInteractionSettings::commandShutdownQemu()
{
    QMPInteraction::commandQmp(QMPCommands::Quit);
}
