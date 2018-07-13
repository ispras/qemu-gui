#include "DeviceBus.h"
#include "DeviceStorage.h"

DeviceBus::DeviceBus(const QString &n, Device *parent)
    : Device(n, parent)
{

}

const char DeviceBusIde::typeName[] = "DeviceBusIde";

DeviceBusIde::DeviceBusIde(int id, DeviceIdeController *parent)
    : DeviceBus(QString("ide.%1").arg(id), parent)
{

}
