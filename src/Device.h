#ifndef DEVICE_H
#define DEVICE_H

#include <QWidget>
#include <QtWidgets>

class Device : public QObject
{
    Q_OBJECT
public:
    Device(const QString &n, Device *parent = 0);

    typedef QList<Device *> Devices;

    void addDevice(Device *dev);
    const Devices &getDevices() const { return devices; }
    QString getDescription() const;

    void save(QXmlStreamWriter &xml) const;

private:
    QString name;
    Devices devices;
};

#endif // DEVICE_H
