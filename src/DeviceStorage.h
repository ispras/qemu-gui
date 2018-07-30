#ifndef DEVICESTORAGE_H
#define DEVICESTORAGE_H

#include "Device.h"

/*class DeviceStorage : public Device
{
public:
    DeviceStorage(const QString &n, Device *parent);
};*/

class DeviceStorageController : public Device
{
public:
    DeviceStorageController() {}
    DeviceStorageController(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
};

class DeviceIdeController : public DeviceStorageController
{
public:
    static const char typeName[];

    DeviceIdeController() { initDefault(); }
    DeviceIdeController(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }

private:
    void initDefault();
};

class DeviceStorage : public Device
{
public:
    DeviceStorage() {}
    DeviceStorage(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
};

class DeviceIdeHd : public DeviceStorage
{
Q_OBJECT

public:
    static const char typeName[];

    DeviceIdeHd() {}
    DeviceIdeHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption();

private:
    QString image;

    private slots:
    void editImage();

signals:
    void newImageSet(QString);
};

#endif // DEVICESTORAGE_H
