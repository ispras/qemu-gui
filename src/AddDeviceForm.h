#ifndef ADDDEVICEFORM_H
#define ADDDEVICEFORM_H

#include <QtWidgets>
#include "Device.h"
#include <QMap>

class AddDeviceForm : public QWidget
{
    Q_OBJECT

public:
    AddDeviceForm(const Device *device);
    ~AddDeviceForm();

    int getAddDevicesCount();

public slots:
    void addDevice();

private:
    QListWidget *deviceList;
    Devices addDevices;
    QMap <QString, Device *> devices;
    bool isDeleted = false;
    

private slots:
    void addNewDevice();

signals:
    void deviceWantsToAdd(Device *);


};

#endif // ADDDEVICEFORM_H
