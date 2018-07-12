#include "DeviceBus.h"
#include "DeviceStorage.h"

DeviceBus::DeviceBus(const QString &n, Device *parent)
    : Device(n, parent)
{

}

DeviceBusIde::DeviceBusIde(int id, DeviceIdeController *parent)
    : DeviceBus(QString("ide.%1").arg(id), parent)
{

}
