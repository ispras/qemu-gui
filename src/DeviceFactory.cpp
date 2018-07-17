#include "DeviceFactory.h"
#include "DeviceStorage.h"

namespace DeviceFactory
{

Device *createDevice(QStringRef name)
{
    if (name == DeviceIdeController::typeName)
        return new DeviceIdeController();
    if (name == DeviceIdeHd::typeName)
        return new DeviceIdeHd();

    return new Device();
}

}
