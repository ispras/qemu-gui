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
    virtual QString getCommandLineOption();

private:
    QString size; /* in megabytes */
};





#endif // DEVICESYSTEM_H


