#include "DeviceBus.h"
#include "DeviceStorage.h"

DeviceBus::DeviceBus(const QString &n, Device *parent)
    : Device(n, parent)
{

}

const char DeviceBusIde::typeName[] = "DeviceBusIde";

DeviceBusIde::DeviceBusIde(int n, DeviceIdeController *parent)
    : DeviceBus(QString("ide.%1").arg(n), parent), num(n)
{
}


//const char DeviceBusPci::typeName[] = "DeviceBusPci";

//DeviceBusPci::DeviceBusPci(int n, DevicePciController *parent)
//    : DeviceBus(QString("pci.%1").arg(num), parent), num(n)
//{
//
//}
