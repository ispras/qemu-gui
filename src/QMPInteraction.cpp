#include "QMPInteraction.h"

QMPInteraction::QMPInteraction(QObject *parent)
    : QObject(parent)
{
    prepareCommands();
}

QMPInteraction::QMPInteraction(QObject *parent, int port)
    : QMPInteraction(parent)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(readTerminal()));
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
    cmdMap.insert(QMPCommands::QmpCapabilities, { "qmp_capabilities", "", NULL });
    cmdMap.insert(QMPCommands::QueryStatus,{ "query-status", "", &QMPInteraction::queryStatus_cb });
    cmdMap.insert(QMPCommands::QueryMachines, { "query-machines", "", &QMPInteraction::machine_cb });
    cmdMap.insert(QMPCommands::QueryCpuDefinitions, { "query-cpu-definitions", "", &QMPInteraction::cpu_cb });
    cmdMap.insert(QMPCommands::QomListTypes, { "qom-list-types",
        ",\"arguments\":{\"abstract\":true, \"implements\":\"device\"}",
        &QMPInteraction::listDevices_cb });
    cmdMap.insert(QMPCommands::DeviceListProperties, { "device-list-properties", "?", &QMPInteraction::listProperties_cb });
}

bool QMPInteraction::isEvent(QJsonObject object)
{
    QJsonValue json_command = object["event"];
    qDebug() << json_command;
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
    QJsonDocument qmp_message = QJsonDocument::fromJson(message);
    qDebug() << qmp_message;
    if (qmp_message.isNull())
    {
        return false;
    }
    QJsonObject obj = qmp_message.object();

    if (cbQueue.count())
    {
        (this->*(cbQueue.first()))(obj);
    }
    return true;
}

void QMPInteraction::readTerminal()
{
    QByteArray message = socket.readAll();
    QList<QByteArray> messageBuffer = message.split('\n');
    foreach(QByteArray message, messageBuffer)
    {
        whatSaidQmp(message);
    }
}

void QMPInteraction::connectedSocket()
{
    commandQmp(QMPCommands::QmpCapabilities);
    commandQmp(QMPCommands::QueryStatus);
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
    socket.write(commandAll);
}


/******************************************************************************
* QMP Interaction Settings                                                    *
******************************************************************************/


QMPInteractionSettings::QMPInteractionSettings(QObject *parent, int port)
    : QMPInteraction(parent)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(readTerminal()));
    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSocket()));

    isQmpReady = false;

    socket.connectPort(port);
    messageBegin = "";

    commandsQueue.append({ QMPCommands::QueryMachines,
                        QMPCommands::QueryCpuDefinitions,
                        QMPCommands::QomListTypes,
    });
}

QMPInteractionSettings::~QMPInteractionSettings()
{
}


void QMPInteractionSettings::readJsonArray(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        infoList.clear();
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            QString name = v.toObject()["name"].toString();
            bool isDefault = v.toObject()["is-default"].toBool();
            if (isDefault)
            {
                infoList.push_front(name);
            }
            else
            {
                infoList.append(name);
            }
        }
        emit readyInfo(infoList, commandsQueue.count());
        messageBegin = "";
        cbQueue.pop_front();
    }
}

void QMPInteractionSettings::machine_cb(QJsonObject object)
{
    readJsonArray(object);
}

void QMPInteractionSettings::cpu_cb(QJsonObject object)
{
    readJsonArray(object);
}

void QMPInteractionSettings::listDevices_cb(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        infoList.clear();
        QJsonArray arrMessage = jv.toArray();
        for (const QJsonValue &v : arrMessage)
        {
            bool isAbstract = v.toObject()["abstract"].toBool();
            QString deviceName = v.toObject()["name"].toString();
            if (!isAbstract)
            {
                infoList.append(deviceName);
            }
        }
        cbQueue.pop_front();
        QMPInteraction::commandQmp(QMPCommands::DeviceListProperties,
            getParamDevListProperties(infoList.first()));
    }
}

void QMPInteractionSettings::listProperties_cb(QJsonObject object)
{
    QJsonValue jv = object.value("return");
    if (jv.isArray())
    {
        QJsonArray arrMessage = jv.toArray();
        QStringList parentList;
        for (const QJsonValue &v : arrMessage)
        {
            QString deviceName = v.toObject()["name"].toString();
            if (deviceName.compare("netdev") == 0)
            {
                netdevList.append(infoList.first());
            }
        }
        cbQueue.pop_front();

        infoList.removeFirst();
        if (infoList.count())
        {
            QMPInteraction::commandQmp(QMPCommands::DeviceListProperties,
                getParamDevListProperties(infoList.first()));
        }
        else
        {
            emit readyInfo(netdevList, commandsQueue.count());
        }
    }
}

QString QMPInteractionSettings::getParamDevListProperties(const QString &name) const
{
    return ",\"arguments\":{\"typename\":\"" + name + "\"}";
}

void QMPInteractionSettings::readTerminal()
{
    QByteArray message = messageBegin + socket.readAll();
    if (!whatSaidQmp(message))
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
    QMPInteraction::commandQmp(QMPCommands::QmpCapabilities);
}

void QMPInteractionSettings::commandShutdownQemu()
{
    QMPInteraction::commandQmp(QMPCommands::Quit);
}

void QMPInteractionSettings::commandQmp()
{
    QMPInteraction::commandQmp(commandsQueue.first());
    commandsQueue.removeFirst();
}


