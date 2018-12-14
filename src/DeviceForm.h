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

private slots:
    void setController(const QString &name);

};


class DeviceCommandLineForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceCommandLineForm(Device *dev);

private:
    Device *device;
    QTextEdit *cmdLine;
    QLabel *optionalLbl;
    QLineEdit *optionalLine;

private slots:
    void showCmdLine();
    void saveUserOptions();
};

#endif // DEVICEFORM_H
