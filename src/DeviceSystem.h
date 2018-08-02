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

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption();

private:
    QString size; /* in megabytes */
};


class DeviceMemoryForm : public QGroupBox
{
Q_OBJECT

public:
    DeviceMemoryForm(DeviceMemory *dev);

private:
    DeviceMemory *device;

    private slots:
    void sizeChanged(int val);


};


#endif // DEVICESYSTEM_H


