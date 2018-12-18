#ifndef DEVICEFORM_H
#define DEVICEFORM_H

#include <QtWidgets>
#include "DeviceStorage.h"
#include "DeviceSystem.h"

class DeviceCommandLineForm;

class DeviceForm : public QGroupBox
{
    Q_OBJECT

public:
    DeviceForm(Device *dev);

protected:
    void devFormAddWidget(QWidget *widget);
    void devFormAddLayout(QLayout *layout);

private:
    Device *device;
    QVBoxLayout *mainLay;
    DeviceCommandLineForm *cmdWidget;
};


class DeviceStorageForm : public DeviceForm
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


class DeviceScsiControllerForm : public DeviceForm
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


class DeviceMemoryForm : public DeviceForm
{
    Q_OBJECT

public:
    DeviceMemoryForm(DeviceMemory *dev);

private:
    DeviceMemory *device;

    private slots:
    void sizeChanged(int val);
};

#endif // DEVICEFORM_H
