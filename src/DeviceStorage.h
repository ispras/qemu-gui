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

protected:
    virtual bool isRemovable() { return !isCanRemove.compare("true") ? true : false; }
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);

private:
    void initDefault();
    static const char deviceName[];
    QString isCanRemove;
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

protected:
    virtual BusType providesBus() const { return BusType::SCSI; }
    virtual bool isRemovable() { return true; }

private:
    void initDefault();
    static const char deviceName[];
};

class DeviceStorage : public Device
{
public:
    DeviceStorage() {}
    DeviceStorage(const QString &n, Device *parent);

    virtual QString getDeviceTypeName() const = 0;

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual void setImage(const QString &img) = 0;
    virtual QString getImage() const = 0;

private:
    QString getChildImage();
    void setImageChild(const QString &img) const;
};

class DeviceIdeHd : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceIdeHd();
    DeviceIdeHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    virtual BusType needsBus() const { return BusType::IDE; }

    QString getImage() const { return image; }
    void setImage(const QString &img) { image = img; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();
    virtual bool isRemovable() { return true; }

private:
    static const char deviceName[];
    QString image;
};


class DeviceIdeCdrom : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceIdeCdrom();
    DeviceIdeCdrom(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    virtual BusType needsBus() const { return BusType::IDE; }

    QString getImage() const { return image; }
    void setImage(const QString &img) { image = img; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();
    virtual bool isRemovable() { return true; }

private:
    static const char deviceName[];
    QString image;
};


class DeviceScsiHd : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceScsiHd();
    DeviceScsiHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    virtual BusType needsBus() const { return BusType::SCSI; }

    QString getImage() const { return image; }
    void setImage(const QString &img) { image = img; }

protected:
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();
    virtual bool isRemovable() { return true; }

private:
    static const char deviceName[];
    QString image;
};

#endif // DEVICESTORAGE_H
