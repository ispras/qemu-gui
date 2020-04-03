#ifndef QMPINTERACTION_H
#define QMPINTERACTION_H

#include <QtCore/QtCore>
#include "QemuSocket.h"
#include "config/PlatformInfo.h"

enum class QMPCommands : int 
{ 
    Continue,
    Stop,
    Quit,
    QmpCapabilities,
    QueryStatus,
    QueryMachines,
    QueryCpuDefinitions,
    QomListTypes,
    DeviceListProperties
};

class QMPInteraction : public QObject
{
    Q_OBJECT

public:
    QMPInteraction(QObject *parent);
    QMPInteraction(QObject *parent, int port);
    ~QMPInteraction();

    struct QmpCommand
    {
        QString command;
        QString params;
        void (QMPInteraction::*callback)(QJsonObject);
    };

protected:
    QemuSocket socket;
    QByteArray messageBegin;
    QList<void (QMPInteraction::*)(QJsonObject)> cbQueue;
    QMap<QMPCommands, QmpCommand> cmdMap;
    QList<QMPCommands> commandsQueue;

private:
    void dummy_cb(QJsonObject object);
    bool isEvent(QJsonObject object);
    void queryStatus_cb(QJsonObject object);
    void stop_cb(QJsonObject object);
    void continue_cb(QJsonObject object);
    
protected:
    void prepareCommands();
    bool whatSaidQmp(QByteArray message);
    virtual void machine_cb(QJsonObject object) {}
    virtual void cpu_cb(QJsonObject object) {}
    virtual void listDevices_cb(QJsonObject object) {}
    virtual void listProperties_cb(QJsonObject object) {}

public slots:
    void readTerminal();
    void connectedSocket();
    void commandQmp(QMPCommands cmd, const QString &specParams = "");

signals :
    void qemuResumed();
    void qemuStopped();
    void qemuRunningStatus(bool runStatus);
    void ready();
};


class QMPInteractionSettings : public QMPInteraction
{
    Q_OBJECT

public:
    QMPInteractionSettings(QObject *parent, int port, PlatformInfo *pi);

private:
    PlatformInfo *platformInfo;
    QStringList devices;

private:
    virtual void machine_cb(QJsonObject object);
    virtual void cpu_cb(QJsonObject object);
    virtual void listDevices_cb(QJsonObject object);
    virtual void listProperties_cb(QJsonObject object);

    QString getParamDevListProperties(const QString &name) const;

public slots:
    void commandShutdownQemu();
};


#endif // QMPINTERACTION_H

