#ifndef QMPINTERACTION_H
#define QMPINTERACTION_H

#include <QWidget>
#include <QtWidgets>
#include "QemuSocket.h"

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
    QList<QmpCommand> commands;
    QList<void (QMPInteraction::*)(QJsonObject)> cbQueue;

private:
    bool isEvent(QJsonObject object);
    void queryStatus_cb(QJsonObject object);
    void stop_cb(QJsonObject object);
    void continue_cb(QJsonObject object);
    
protected:
    void prepareCommands();
    void whatSaidQmp(QByteArray message);
    int getCallbackByCmdName(const QString &cmd);
    virtual void machine_cb(QJsonObject object) {}
    virtual void cpu_cb(QJsonObject object) {}
    virtual void listDevices_cb(QJsonObject object) {}
    virtual void listProperties_cb(QJsonObject object) {}

public slots:
    void read_terminal();
    void connectedSocket();
    void commandQmp(const QString &command);

signals :
    void qemu_resumed();
    void qemu_stopped();
    void qemuRunningStatus(bool runStatus);
};


class QMPInteractionSettings : public QMPInteraction
{
    Q_OBJECT

public:
    QMPInteractionSettings(QObject *parent, int port);
    ~QMPInteractionSettings();

private:
    bool isQmpReady;
    QStringList infoList;
    QStringList netdevList;
    QByteArray messageBegin;
    QList<QString> commandsQueue;

private:
    bool whatSaidQmp(QByteArray message);

    void readJsonArray(QJsonObject object);
    virtual void machine_cb(QJsonObject object);
    virtual void cpu_cb(QJsonObject object);
    virtual void listDevices_cb(QJsonObject object);
    virtual void listProperties_cb(QJsonObject object);

public slots:
    void read_terminal();
    void connectedSocket();

    void commandShutdownQemu();
    void commandQmp();

signals:
    void readyInfo(const QStringList &, bool);
    void qmpConnected();

};


#endif // QMPINTERACTION_H

