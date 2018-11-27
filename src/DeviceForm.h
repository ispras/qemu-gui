#ifndef DEVICEFORM_H
#define DEVICEFORM_H

#include <QtWidgets>
#include "DeviceStorage.h"


class DeviceIdeHdForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceIdeHdForm(DeviceIdeHd *dev);

private:
    DeviceIdeHd *device;

    private slots:
    void editImage();

signals:
    void newImageSet(QString);
    void newDiskCompleted(QString);
};


class DeviceIdeCdromForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceIdeCdromForm(DeviceIdeCdrom *dev);

private:
    DeviceIdeCdrom *device;

    private slots:
    void editImage();

signals:
    void newImageSet(QString);
    void newDiskCompleted(QString);
};




#endif // DEVICEFORM_H
