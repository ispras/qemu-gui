#ifndef DEVICESYSTEM_H
#define DEVICESYSTEM_H

#include "Device.h"
#include "QemuGUICommon.h"


class DeviceMemory : public Device
{
public:
    static const char typeName[];

    DeviceMemory() {}
    DeviceMemory(const QString &memSize, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    void setSize(const QString &newSize) { size = newSize; }
    int getSize() const { return size.toInt(); }

    virtual BusType needsBus() const { return BusType::System; }

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual QString getDeviceInfo();

private:
    QString size; /* in megabytes */
};


class DeviceSystem : public Device
{
public:
    DeviceSystem() {}
    DeviceSystem(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
    virtual bool isDeviceInvisible() { return true; }
    virtual BusType needsBus() const { return BusType::System; }
    QString getName() const { return name; }

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    void setName(const QString &newName) { name = newName; }

private:
    QString name;
};

class DeviceMachine : public DeviceSystem
{
public:
    static const char typeName[];

    DeviceMachine() {}
    DeviceMachine(const QString &machineName, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual QString getDeviceInfo();
};


class DeviceCpu : public DeviceSystem
{
public:
    static const char typeName[];

    DeviceCpu() {}
    DeviceCpu(const QString &cpuName, Device *parent);

    virtual QWidget *getEditorForm();
    virtual QString getDeviceTypeName() const { return typeName; }
    virtual bool isDeviceInvisible() { return false; }

    const QStringList getCpuModels() const;
    void setCpuModel(const QString &name) { setName(name); }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual QString getDeviceInfo();
};




#endif // DEVICESYSTEM_H


