#ifndef DEVICEFORM_H
#define DEVICEFORM_H

#include <QtWidgets>
#include "DeviceStorage.h"

class DeviceStorageForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceStorageForm(DeviceStorage *dev);

private:
    DeviceStorage *device;

protected slots:
    void editImage();

signals:
    void newImageSet(QString);
    void newDiskCompleted(QString);
};


class DeviceScsiControllerForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceScsiControllerForm(DeviceScsiController *dev);

private:
    DeviceScsiController *device;

public slots:
    void setController(const QString &name);

};

#endif // DEVICEFORM_H
