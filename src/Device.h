#ifndef DEVICE_H
#define DEVICE_H

#include <QWidget>
#include <QtWidgets>

class CommandLineParameters;

class Device : public QObject
{
    Q_OBJECT
public:
    Device() {}
    Device(const QString &n, Device *parent = 0);

    typedef QList<Device *> Devices;

    void addDevice(Device *dev);
    const Devices &getDevices() const { return devices; }
    QString getDescription() const;
    QString getCommandLine(CommandLineParameters &cmdParams);

    void save(QXmlStreamWriter &xml) const;
    void read(QXmlStreamReader &xml);

    virtual QString getDeviceTypeName() const { return "Device"; }
    virtual QWidget *getEditorForm() { return NULL; }
    virtual QStringList getDeviceListToAdd() { return QStringList(); }
    virtual bool checkValidationDeviceInfo() { return true; }

protected:
    virtual void saveParameters(QXmlStreamWriter &xml) const {}
    virtual void readParameters(QXmlStreamReader &xml) {}
    virtual QString getCommandLineOption(CommandLineParameters &cmdParams) { return ""; }

private:
    QString name;
    Devices devices;
};

#endif // DEVICE_H
