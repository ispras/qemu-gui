#ifndef ADDDEVICEFORM_H
#define ADDDEVICEFORM_H

#include <QtWidgets>
#include "Device.h"

class AddDeviceForm : public QWidget
{
    Q_OBJECT

public:
    AddDeviceForm(const Devices &listDevice);
    ~AddDeviceForm();

public slots:
    void addDevice();

private:
    QListWidget *deviceList;
    

private slots:
    void addNewDevice();

signals:
    void deviceWantsToAdd(const QString &);


};

#endif // ADDDEVICEFORM_H
