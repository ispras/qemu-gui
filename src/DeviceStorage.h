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
    virtual BusType needsBus() const { return BusType::PCI; }

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

public:
    static const char typeName[];

    DeviceIdeHd() {}
    DeviceIdeHd(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    virtual BusType needsBus() const { return BusType::IDE; }

    void setNewHDD(const QString &imageName) { image = imageName; }
    QString getImage() const { return image; }

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();

private:
    QString image;
};


class DeviceIdeCdrom : public DeviceStorage
{

public:
    static const char typeName[];

    DeviceIdeCdrom() {}
    DeviceIdeCdrom(const QString &img, Device *parent);

    virtual QString getDeviceTypeName() const { return typeName; }
    virtual QWidget *getEditorForm();

    virtual BusType needsBus() const { return BusType::IDE; }

    void setCDImage(const QString &imageName) { image = imageName; }
    QString getImage() const { return image; }

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const;
    virtual void readParameters(QXmlStreamReader &xml);
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams);
    virtual bool isDeviceValid();

private:
    QString image;
};


#endif // DEVICESTORAGE_H
