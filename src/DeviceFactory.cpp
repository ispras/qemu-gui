#include "DeviceFactory.h"
#include "DeviceStorage.h"
#include "DeviceSystem.h"
#include "DeviceUsb.h"

namespace DeviceFactory
{

Device *createDevice(const QString &name)
{
    auto create = Internal::getDeviceRegistry().value(name);
    if (create)
        return create();

    return new Device();
}

namespace Internal
{

DeviceRegistry &getDeviceRegistry()
{
    static DeviceRegistry reg;
    return reg;
}

}

}
