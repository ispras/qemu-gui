#include "DeviceFactory.h"
#include "DeviceStorage.h"
#include "DeviceSystem.h"
#include "DeviceUsb.h"

namespace DeviceFactory
{

Device *createDevice(QStringRef name)
{
    if (name == DeviceIdeController::typeName)
        return new DeviceIdeController();
    if (name == DeviceIdeHd::typeName)
        return new DeviceIdeHd();
    if (name == DeviceMemory::typeName)
        return new DeviceMemory();
    if (name == DeviceUsb::typeName)
        return new DeviceUsb();

    return new Device();
}

}
