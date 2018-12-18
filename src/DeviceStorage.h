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

    DeviceIdeController();
    DeviceIdeController(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual BusType needsBus() const { return BusType::PCI; }

private:
    void initDefault();
    static const char deviceName[];
};

class DevicePciController : public DeviceStorageController
{
public:
    static const char typeName[];

    DevicePciController() { initDefault(); }
    DevicePciController(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual BusType needsBus() const { return BusType::System; }

protected:
    virtual BusType providesBus() const { return BusType::PCI; }

private:
    void initDefault();
};

class DeviceScsiController : public DeviceStorageController
{
public:
    static const char typeName[];

    DeviceScsiController();
    DeviceScsiController(Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual BusType needsBus() const { return BusType::PCI; }
    virtual QWidget *getEditorForm();

    const QStringList &getControllers() const;
    void setController(const QString &name) { controller = name; }
    QString getCurrentController() const { return controller; }

protected:
    virtual BusType providesBus() const { return BusType::SCSI; }
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);

private:
    void initDefault();
    static const char deviceName[];
    QString controller;
};

class DeviceStorage : public Device
{
public:
    DeviceStorage() {}
    DeviceStorage(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;
    virtual void setImage(const QString &img) { image = img; };
    virtual QString getImage() const { return image; };
    virtual QWidget *getEditorForm();

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);

private:
    QString image;
};

class DeviceIdeHd : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceIdeHd();
    DeviceIdeHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual BusType needsBus() const { return BusType::IDE; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();

private:
    static const char deviceName[];
};


class DeviceIdeCdrom : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceIdeCdrom();
    DeviceIdeCdrom(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual BusType needsBus() const { return BusType::IDE; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();

private:
    static const char deviceName[];
};


class DeviceScsiHd : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceScsiHd();
    DeviceScsiHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual BusType needsBus() const { return BusType::SCSI; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();

private:
    static const char deviceName[];
};

#endif // DEVICESTORAGE_H
