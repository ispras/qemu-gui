#include "QMPInteraction.h"

QMPInteraction::QMPInteraction(QObject *parent)
    : QObject(parent)
{
}

QMPInteraction::QMPInteraction(QObject *parent, int port)
    : QObject(parent)
{
    connect(&socket, SIGNAL(readyRead()), this, SLOT(read_terminal()));
    connect(&socket, SIGNAL(connected()), this, SLOT(connectedSocket()));

    socket.connectPort(port);
    prepareCommands();
}

QMPInteraction::~QMPInteraction()
{
}


void QMPInteraction::prepareCommands()
{
    commands.append({ "query-status", "", &QMPInteraction::queryStatus_cb });
    commands.append({ "stop", "", &QMPInteraction::stop_cb });
    commands.append({ "cont", "", &QMPInteraction::continue_cb });
    commands.append({ "query-machines", "", &QMPInteraction::machine_cb });
    commands.append({ "query-cpu-definitions", "", &QMPInteraction::cpu_cb });
    commands.append({ "qom-list-types", ",\"arguments\":{\"abstract\":true}",
        &QMPInteraction::listDevices_cb });
    commands.append({ "device-list-properties", "?", &QMPInteraction::listProperties_cb });
}

int QMPInteraction::getCallbackByCmdName(const QString &cmd)
{
    for (int i = 0; i < commands.count(); i++)
    {
        if (commands.at(i).command.compare(cmd) == 0)
        {
            return i;
        }
    }
    return -1;
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
        emit qemu_stopped();
    }
}

void QMPInteraction::continue_cb(QJsonObject object)
{
    if (isEvent(object))
    {
        emit qemu_resumed();
    }
}

void QMPInteraction::whatSaidQmp(QByteArray message)
{
    qDebug() << "QMP: " << message;
    QJsonDocument qmp_message = QJsonDocument::fromJson(message);
    QJsonObject obj = qmp_message.object();

    if (cbQueue.count())
    {
        (this->*(cbQueue.first()))(obj);
    }
}

void QMPInteraction::read_terminal()
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
    commandQmp("qmp_capabilities");
    commandQmp("query-status");
}

void QMPInteraction::commandQmp(const QString &cmd)
{
    int indexCmd = getCallbackByCmdName(cmd);
    QString params = "";
    if (indexCmd != -1)
    {
        cbQueue.append(commands.at(indexCmd).callback);
        params = commands.at(indexCmd).params;
    }

    QByteArray command = "{ \"execute\":\"" + cmd.toLocal8Bit() + "\""
        + params.toLocal8Bit() + "}";
    socket.write(command);
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

    commandsQueue.append({ "query-machines",
                        "query-cpu-definitions",
                        "qom-list-types",
    });
    prepareCommands();
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
            infoList.append(name);
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
            QString parentName = v.toObject()["parent"].toString();
            if (!isAbstract && parentName.compare("pci-device") == 0)
            {
                infoList.append(deviceName);
            }
        }
        cbQueue.pop_front();
        commandsQueue.append("device-list-properties");
        commandQmp();
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
            commandsQueue.append("device-list-properties");
            commandQmp();
        }
        else
        {
            emit readyInfo(netdevList, commandsQueue.count());
        }
    }
}

bool QMPInteractionSettings::whatSaidQmp(QByteArray message)
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

void QMPInteractionSettings::read_terminal()
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
    QMPInteraction::commandQmp("qmp_capabilities");
}

void QMPInteractionSettings::commandShutdownQemu()
{
    QMPInteraction::commandQmp("quit");
}

void QMPInteractionSettings::commandQmp()
{
    QString cmd = commandsQueue.first();
    int indexCmd = getCallbackByCmdName(cmd);
    QString params = "";
    if (indexCmd != -1)
    {
        params = commands.at(indexCmd).params;
        cbQueue.append(commands.at(indexCmd).callback);
        if (params.compare("?") == 0)
        {
            params = ",\"arguments\":{\"typename\":\"" + infoList.first() + "\"}";
        }
    }
    QByteArray command = "{ \"execute\":\"" + cmd.toLocal8Bit() + "\""
        + params.toLocal8Bit() + "}";
    socket.write(command);
    commandsQueue.removeFirst();
}


