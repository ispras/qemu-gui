#include "DeviceStorage.h"
#include "DeviceBus.h"

DeviceStorageController::DeviceStorageController(const QString &n, Device *parent)
    : Device(n, parent)
{

}

DeviceIdeController::DeviceIdeController(Device *parent)
    : DeviceStorageController("IDE", parent)
{
    new DeviceBusIde(0, this);
    new DeviceBusIde(1, this);
}
